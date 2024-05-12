#include "plugin.hpp"
#include "sts-base.hpp"
#include "Ticker.hpp"
#include <math.h>

struct Ticker : Module
{
	enum ParamId
	{
		MSR_BPM_PARAM,
		MSR_RESET_BTN_PARAM,
		MSR_RUN_BTN_PARAM,
		MSR_GATE_LEN_PARAM,
		CLK1_DIV_PARAM,
		CLK1_PHASE_PARAM,
		CLK1_GATE_LEN_PARAM,
		CLK1_SWING_PARAM,
		PARAMS_LEN
	};
	enum InputId
	{
		MSR_BPM_IN_INPUT,
		MSR_RESET_IN_INPUT,
		MSR_RUN_IN_INPUT,
		MSR_GATE_LEN_IN_INPUT,
		CLK1_PHASE_IN_INPUT,
		CLK1_GATE_LEN_IN_INPUT,
		CLK1_SWING_IN_INPUT,
		INPUTS_LEN
	};
	enum OutputId
	{
		MSR_GATE_OUTPUT,
		MSR_TRIGGER_OUTPUT,
		MSR_RESET_OUTPUT,
		MSR_RUN_OUTPUT,
		CLK1_GATE_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId
	{
		MSR_RESET_LIGHT,
		MSR_RUN_LIGHT,
		MSR_PULSE_LIGHT,
		CLK1_PULSE_LIGHT,
		LIGHTS_LEN
	};

	// Some class-wide parameters

#define MIN_BPM_PARAM 10.f	// minimum BPM value
#define MAX_BPM_PARAM 400.f // maximum BPM value

	// Triggers to detect an button push on run or reset signal
	dsp::BooleanTrigger runButtonTrigger;
	dsp::BooleanTrigger resetButtonTrigger;

	// Triggers to detect an external run or reset signal
	dsp::SchmittTrigger runTrigger;
	dsp::SchmittTrigger resetTrigger;

	dsp::PulseGenerator runPulse;
	dsp::PulseGenerator msr_clockPulse;
	dsp::PulseGenerator clk1_clockPulse;
	dsp::PulseGenerator resetPulse;

	bool is_Running = false; // is the clock running?

	bool runButtonTriggered = false;
	bool runTriggered = false;
	bool resetButtonTriggered = false;
	bool resetTriggered = false;
	// Vars to trigger Run, Reset and Master Clock Trigger outputs
	bool resetGate = false;
	bool runGate = false;
	bool clockGate = false;

	const float One_Hz = 1.f / 60.f; // Factor to convert BPM to Hertz

	int master_BPM = 120;			  // current Master BPM value param
	int master_BPM_Old = 120;		  // Old Master BPM value
	bool master_Gate_started = false; // Toggle to see if we started a new master pulse
	float master_Freq = 2.f;		  // current Master Frequence = BPM / 60
	float master_Phase = 0.f;		  // holds the phase of the Master Clock
	float master_Gate_Len = 50.f;	  // Master Gate length in %
	float master_Gate_Voltage;		  // Output voltage for Master Gate

	float clk1_Divider = 1.f;		 // current CLK1 Divider param
	float clk1_Divider_Mapped = 1.f; // current CLK1 Divider param mapped to actual divider factor
	float clk1_Divider_Old = 1.f;	 // Old CLK1 Divider
	bool clk1_Gate_started = false;	 // Toggle to see if we started a new clk1 gate
	float clk1_Freq = 2.f;			 // current CLK1 Frequency
	float clk1_Phase = 0.f;			 // holds the phase of CLK1
	float clk1_Gate_Len = 50.f;		 // CLK1 Gate length in %
	float clk1_Phase_Shift = 0.f;	 // Phase shift / delay of the pulse
	float clk1_Swing_Amount = 0.f;	 // Amount of Swing to apply (0..10%)
	float clk1_Swing_value = 0.f;
	float clk1_Gate_Voltage = 0.f; // Output voltage for CLK1 Gate

	// A clock is basically a pulse, so using a modified part of the Pulse_VCO code here
	// phase_shift is used to delay/swing the pulse, pulse_width is a %%
	float STS_My_Pulse(float phase, float phase_shift, float pulse_width)
	{
		float local_phase;

		// Displace by phase shift, map into 0..1. Note that phase_shift can be negative in this code
		local_phase = (phase + phase_shift) * 100.f;
		if (local_phase < 0.f)
			local_phase += 100.f;
		if (local_phase > 100.f)
			local_phase -= 100.f;
		if (local_phase < pulse_width)
			return (10.f);
		else
			return (0.f);
	}

	// This is an Initialize, not a RESET button push
	void onReset() override
	{
		// Set defaults
		master_BPM = master_BPM_Old = 120;
		clk1_Divider = clk1_Divider_Old = 2.f;
		is_Running = false;
		master_Phase = 0.f;
		clk1_Phase = 0.f;
		clk1_Phase_Shift = 0.f;
		clk1_Swing_Amount = 0.f;
		clk1_Swing_value = 0.f;

		// Reset some other vars
		clockGate = false;
		runGate = false;
		resetGate = false;
		master_Gate_started = false;

		// Reset all triggers
		resetPulse.reset();
		msr_clockPulse.reset();
		runPulse.reset();
		clk1_clockPulse.reset();

		// Reset all outputs & lights
		outputs[MSR_GATE_OUTPUT].setVoltage(0.f);
		outputs[MSR_TRIGGER_OUTPUT].setVoltage(0.f);
		outputs[MSR_RESET_OUTPUT].setVoltage(0.f);
		outputs[MSR_RUN_OUTPUT].setVoltage(0.f);

		lights[MSR_RESET_LIGHT].setBrightness(0.f);
		lights[MSR_RUN_LIGHT].setBrightness(0.f);
		lights[MSR_PULSE_LIGHT].setBrightness(0.f);
		lights[MSR_PULSE_LIGHT].setBrightness(0.f);
	}

	Ticker()
	{
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

		// Master Clock Params
		configParam(MSR_BPM_PARAM, 10.f, 400.f, 120.f, "BPM");
		configParam(MSR_GATE_LEN_PARAM, 1.f, 99.f, 50.f, "Gate Length", "%");
		// Master Clock Buttons
		configButton(MSR_RESET_BTN_PARAM, "Reset");
		configButton(MSR_RUN_BTN_PARAM, "Run");
		// Master Clock Inputs
		configInput(MSR_BPM_IN_INPUT, "BPM Voltage (0..10V)");
		configInput(MSR_RESET_IN_INPUT, "Reset Trigger");
		configInput(MSR_RUN_IN_INPUT, "Run Trigger");
		configInput(MSR_GATE_LEN_IN_INPUT, "Gate Length Voltage (0..10V)");
		// Master Clock Outputs
		configOutput(MSR_GATE_OUTPUT, "Master Clock Gate Out");
		configOutput(MSR_TRIGGER_OUTPUT, "Master Clock Trigger Out");
		configOutput(MSR_RESET_OUTPUT, "Reset Out");
		configOutput(MSR_RUN_OUTPUT, "Run Out");

		// Clock 1 Params
		configParam(CLK1_DIV_PARAM, 0.f, 74.f, 39.f, "Divider", "");
		paramQuantities[CLK1_DIV_PARAM]->snapEnabled = true;
		configParam(CLK1_PHASE_PARAM, -1.f, 1.f, 0.f, "Phase shift", " Cycle");
		configParam(CLK1_GATE_LEN_PARAM, 1.f, 99.f, 50.f, "Gate Length", "%");
		configParam(CLK1_SWING_PARAM, 0.f, 5.f, 0.f, "Swing Amount", "%");
		// Clock 1 Inputs
		configInput(CLK1_PHASE_IN_INPUT, "Phase shift Voltage (0..10V)");
		configInput(CLK1_GATE_LEN_IN_INPUT, "Gate Length Voltage (0..10V)");
		configInput(CLK1_SWING_IN_INPUT, "Swing Amount Voltage (0..10V)");
		// Clock 1 Outputs
		configOutput(CLK1_GATE_OUTPUT, "Clock Gate Out");

		onReset();
	}

	void process(const ProcessArgs &args) override
	{
		// Get all the values from the module UI

		// Master Clock
		// BPM Data = 10V mapped to range 10-400 BPM
		if (inputs[MSR_BPM_IN_INPUT].isConnected())
		{
			master_BPM = MIN_BPM_PARAM + (MAX_BPM_PARAM - MIN_BPM_PARAM) * inputs[MSR_BPM_IN_INPUT].getVoltage() * 0.1f;
		}
		else
			master_BPM = (int)params[MSR_BPM_PARAM].getValue();
		// Did we change the BPM? If so, reset all clock phases
		if (master_BPM != master_BPM_Old)
		{
			master_BPM_Old = master_BPM;
			master_Phase = 0.f;
			clk1_Phase = 0.f;
		}
		// Gate Length Data = 10V mapped to range 1-99%
		if (inputs[MSR_GATE_LEN_IN_INPUT].isConnected())
		{
			master_Gate_Len = 1.f + 98.f * inputs[MSR_GATE_LEN_IN_INPUT].getVoltage() * 0.1f;
		}
		else
			master_Gate_Len = (int)params[MSR_GATE_LEN_PARAM].getValue();

		// Clock 1
		// Divider data
		clk1_Divider = params[CLK1_DIV_PARAM].getValue();

		// Did we change the Divider setting? If so, reset the phase as per the master clock and recompute the factor
		if (clk1_Divider != clk1_Divider_Old)
		{
			clk1_Divider_Old = clk1_Divider;
			master_Phase = 0.f;
			clk1_Phase = 0.f;
		}
		// Phase data, 0..10V mapped to 0..1
		if (inputs[CLK1_PHASE_IN_INPUT].isConnected())
		{
			clk1_Phase_Shift = inputs[CLK1_PHASE_IN_INPUT].getVoltage() * 0.1f;
		}
		else
			clk1_Phase_Shift = params[CLK1_PHASE_PARAM].getValue();

		// Gate Length Data = 10V mapped to range 1-99%
		if (inputs[CLK1_GATE_LEN_IN_INPUT].isConnected())
		{
			clk1_Gate_Len = 1.f + 98.f * inputs[CLK1_GATE_LEN_IN_INPUT].getVoltage() * 0.1f;
		}
		else
			clk1_Gate_Len = (int)params[CLK1_GATE_LEN_PARAM].getValue();

		// Swing Amount Data = 10V mapped to range 1-10%
		if (inputs[CLK1_SWING_IN_INPUT].isConnected())
		{
			clk1_Swing_Amount = inputs[CLK1_SWING_IN_INPUT].getVoltage() * 0.1f;
		}
		else
			clk1_Swing_Amount = (int)params[CLK1_SWING_PARAM].getValue() * 0.1f;

		// PROCESSING OF ALL INPUT STARTS HERE
		//
		// Compute Master Clock Frequency and derive the individual clocks
		master_Freq = master_BPM * One_Hz;
		clk1_Freq = master_Freq * clk1_Divider_Mapped;

		// Was Run pressed or a pulse received on Run In?
		runButtonTriggered = runButtonTrigger.process(params[MSR_RUN_BTN_PARAM].getValue());
		runTriggered = runTrigger.process(inputs[MSR_RUN_IN_INPUT].getVoltage(), 0.1f, 2.f);

		if (runButtonTriggered || runTriggered)
		{
			// Start the run trigger
			runPulse.trigger(0.001);
			is_Running ^= true;
		}
		// Check if the run pulse should be sent, and if so, send it out
		runGate = runPulse.process(args.sampleTime);
		outputs[MSR_RUN_OUTPUT].setVoltage((runGate) ? 10.f : 0.f);

		// Was Reset pressed or a pulse received on Reset In?
		resetButtonTriggered = resetButtonTrigger.process(params[MSR_RESET_BTN_PARAM].getValue());
		resetTriggered = resetTrigger.process(inputs[MSR_RESET_IN_INPUT].getVoltage(), 0.1f, 2.f);

		if (resetButtonTriggered || resetTriggered)
		{
			// Start the reset trigger
			resetPulse.trigger(1e-3f);

			// Reset some vars
			is_Running = false;
			master_Phase = 0.f;
			clk1_Phase = 0.f;
		}
		// Check if the reset pulse should be sent, and if so, send it out
		resetGate = resetPulse.process(args.sampleTime);
		outputs[MSR_RESET_OUTPUT].setVoltage((resetGate) ? 10.f : 0.f);

		// Toggle the Reset light. The smaller the delta time, the slower the fade
		lights[MSR_RESET_LIGHT].setBrightnessSmooth(resetGate, 0.25f * args.sampleTime);

		if (is_Running)
		{

			// Accumulate the phase for each clock, make sure it rotates between 0.0 and 1.0
			master_Phase += master_Freq * args.sampleTime;
			if (master_Phase >= 1.f)
				master_Phase -= 1.f;
			clk1_Phase += clk1_Freq * args.sampleTime;
			if (clk1_Phase >= 1.f)
				clk1_Phase -= 1.f;

			// compute the Master Clock signal, which has no swing/phase shift
			master_Gate_Voltage = STS_My_Pulse(master_Phase, 0.0, master_Gate_Len);
			// Start the master clock trigger, if the new pulse started
			if (master_Gate_Voltage > 0.f && !master_Gate_started)
			{
				master_Gate_started = true;
				msr_clockPulse.trigger(1e-3f);
			}
			if (master_Gate_Voltage == 0.f)
				master_Gate_started = false;
			// Check the master trigger pulse
			clockGate = msr_clockPulse.process(args.sampleTime);
			// Output the master gate & trigger
			outputs[MSR_GATE_OUTPUT].setVoltage(master_Gate_Voltage);
			outputs[MSR_TRIGGER_OUTPUT].setVoltage((clockGate) ? 10.f : 0.f);

			// Output the derived clocks as per the pulse width, phase and random swing amount
			clk1_Gate_Voltage = STS_My_Pulse(clk1_Phase, clk1_Phase_Shift + clk1_Swing_value, clk1_Gate_Len);
			// Are we outputting a new CLK1 gate?
			if (clk1_Gate_Voltage > 0.0 && !clk1_Gate_started)
			{
				clk1_Gate_started = true;
				// Recomputeb a new swing value
				clk1_Swing_value = (1.f - 2.f * rack::random::uniform()) * clk1_Swing_Amount;
			}
			if (clk1_Gate_Voltage == 0.f)
				clk1_Gate_started = false;

			outputs[CLK1_GATE_OUTPUT].setVoltage(clk1_Gate_Voltage);

			// Toggle the lights. The smaller the delta time, the slower the fade
			lights[MSR_PULSE_LIGHT].setBrightnessSmooth(master_Gate_Voltage > 0.f, args.sampleTime);
			lights[CLK1_PULSE_LIGHT].setBrightnessSmooth(clk1_Gate_Voltage > 0.f, args.sampleTime);
			lights[MSR_RUN_LIGHT].setBrightness(1.f);
		}
		else // Not running
		{
			// Set phase to 0.0 to avoid the first pulse to be too short/quick
			master_Phase = 0.f;
			clk1_Phase = 0.f;
			master_Gate_started = false;
			clk1_Gate_started = false;

			// Reset all outputs & lights
			outputs[MSR_GATE_OUTPUT].setVoltage(0.f);
			outputs[CLK1_GATE_OUTPUT].setVoltage(0.f);

			lights[MSR_RUN_LIGHT].setBrightness(0.f);
			lights[MSR_PULSE_LIGHT].setBrightness(0.f);
		}
	}
};

struct Ticker_BPM_Display : BPM_Display
{
	Ticker *module;
	void step() override
	{
		int display_BPM = 99;
		if (module)
			display_BPM = module->master_BPM;
		text = string::f("%0*d", 3, display_BPM);
	}
};

struct Ticker_CLK1_Div_Display : CLK1_Div_Display
{
	Ticker *module;

	// Icky code to map the param steps to specific divider or multiplier intervals
	void step() override
	{
		int clk_Divider = 1;
		if (module)
		{
			clk_Divider = (int)module->clk1_Divider;

			switch (clk_Divider)
			{
			case 0:
				module->clk1_Divider_Mapped = 1.f / 96.f;
				text = string::f("/ 96 . 0");
				break;
			case 1:
				module->clk1_Divider_Mapped = 1.f / 96.f;
				text = string::f("/ 96 . 0");
				break;
			case 2:
				module->clk1_Divider_Mapped = 1.f / 96.f;
				text = string::f("/ 96 . 0");
				break;
			case 3:
				module->clk1_Divider_Mapped = 1.f / 96.f;
				text = string::f("/ 96 . 0");
				break;
			case 4:
				module->clk1_Divider_Mapped = 1.f / 92.f;
				text = string::f("/ 92 . 0");
				break;
			case 5:
				module->clk1_Divider_Mapped = 1.f / 88.f;
				text = string::f("/ 88 . 0");
				break;
			case 6:
				module->clk1_Divider_Mapped = 1.f / 84.f;
				text = string::f("/ 84 . 0");
				break;
			case 7:
				module->clk1_Divider_Mapped = 1.f / 80.f;
				text = string::f("/ 80 . 0");
				break;
			case 8:
				module->clk1_Divider_Mapped = 1.f / 76.f;
				text = string::f("/ 76 . 0");
				break;
			case 9:
				module->clk1_Divider_Mapped = 1.f / 72.f;
				text = string::f("/ 72 . 0");
				break;
			case 10:
				module->clk1_Divider_Mapped = 1.f / 68.f;
				text = string::f("/ 68 . 0");
				break;
			case 11:
				module->clk1_Divider_Mapped = 1.f / 64.f;
				text = string::f("/ 64 . 0");
				break;
			case 12:
				module->clk1_Divider_Mapped = 1.f / 60.f;
				text = string::f("/ 60 . 0");
				break;
			case 13:
				module->clk1_Divider_Mapped = 1.f / 56.f;
				text = string::f("/ 56 . 0");
				break;
			case 14:
				module->clk1_Divider_Mapped = 1.f / 52.f;
				text = string::f("/ 52 . 0");
				break;
			case 15:
				module->clk1_Divider_Mapped = 1.f / 48.f;
				text = string::f("/ 48 . 0");
				break;
			case 16:
				module->clk1_Divider_Mapped = 1.f / 44.f;
				text = string::f("/ 44 . 0");
				break;
			case 17:
				module->clk1_Divider_Mapped = 1.f / 40.f;
				text = string::f("/ 40 . 0");
				break;
			case 18:
				module->clk1_Divider_Mapped = 1.f / 36.f;
				text = string::f("/ 36 . 0");
				break;
			case 19:
				module->clk1_Divider_Mapped = 1.f / 32.f;
				text = string::f("/ 32 . 0");
				break;
			case 20:
				module->clk1_Divider_Mapped = 1.f / 28.f;
				text = string::f("/ 28 . 0");
				break;
			case 21:
				module->clk1_Divider_Mapped = 1.f / 24.f;
				text = string::f("/ 24 . 0");
				break;
			case 22:
				module->clk1_Divider_Mapped = 1.f / 20.f;
				text = string::f("/ 20 . 0");
				break;
			case 23:
				module->clk1_Divider_Mapped = 1.f / 18.f;
				text = string::f("/ 18 . 0");
				break;
			case 24:
				module->clk1_Divider_Mapped = 1.f / 16.f;
				text = string::f("/ 16 . 0");
				break;
			case 25:
				module->clk1_Divider_Mapped = 1.f / 12.f;
				text = string::f("/ 12 . 0");
				break;
			case 26:
				module->clk1_Divider_Mapped = 1.f / 10.f;
				text = string::f("/ 10 . 0");
				break;
			case 27:
				module->clk1_Divider_Mapped = 1.f / 9.f;
				text = string::f("/ 9 . 0");
				break;
			case 28:
				module->clk1_Divider_Mapped = 1.f / 8.f;
				text = string::f("/ 8 . 0");
				break;
			case 29:
				module->clk1_Divider_Mapped = 1.f / 7.f;
				text = string::f("/ 7 . 0");
				break;
			case 30:
				module->clk1_Divider_Mapped = 1.f / 6.f;
				text = string::f("/ 6 . 0");
				break;
			case 31:
				module->clk1_Divider_Mapped = 1.f / 5.f;
				text = string::f("/ 5 . 0");
				break;
			case 32:
				module->clk1_Divider_Mapped = 1.f / 4.f;
				text = string::f("/ 4 . 0");
				break;
			case 33:
				module->clk1_Divider_Mapped = 1.f / 3.5f;
				text = string::f("/ 3 . 5");
				break;
			case 34:
				module->clk1_Divider_Mapped = 1.f / 3.f;
				text = string::f("/ 3 . 0");
				break;
			case 35:
				module->clk1_Divider_Mapped = 1.f / 2.5f;
				text = string::f("/ 2 . 5");
				break;
			case 36:
				module->clk1_Divider_Mapped = 1.f / 2.f;
				text = string::f("/ 2 . 0");
				break;
			case 37:
				module->clk1_Divider_Mapped = 1.f / 1.5f;
				text = string::f("/ 1 . 5");
				break;
			case 38:
				module->clk1_Divider_Mapped = 1.f / 1.333333f;
				text = string::f("/ 1 . 33");
				break;
			case 39:
				module->clk1_Divider_Mapped = 1.f;
				text = string::f("x 1 . 0");
				break;
			case 40:
				module->clk1_Divider_Mapped = 1.333333f;
				text = string::f("x 1 . 33");
				break;
			case 41:
				module->clk1_Divider_Mapped = 1.5f;
				text = string::f("x 1 . 5");
				break;
			case 42:
				module->clk1_Divider_Mapped = 2.f;
				text = string::f("x 2 . 0");
				break;
			case 43:
				module->clk1_Divider_Mapped = 2.5f;
				text = string::f("x 2 . 5");
				break;
			case 44:
				module->clk1_Divider_Mapped = 3.f;
				text = string::f("x 3 . 0");
				break;
			case 45:
				module->clk1_Divider_Mapped = 3.5f;
				text = string::f("x 3 . 5");
				break;
			case 46:
				module->clk1_Divider_Mapped = 4.f;
				text = string::f("x 4 . 0");
				break;
			case 47:
				module->clk1_Divider_Mapped = 5.f;
				text = string::f("x 5 . 0");
				break;
			case 48:
				module->clk1_Divider_Mapped = 6.f;
				text = string::f("x 6 . 0");
				break;
			case 49:
				module->clk1_Divider_Mapped = 7.f;
				text = string::f("x 7 . 0");
				break;
			case 50:
				module->clk1_Divider_Mapped = 8.f;
				text = string::f("x 8 . 0");
				break;
			case 51:
				module->clk1_Divider_Mapped = 9.f;
				text = string::f("x 9 . 0");
				break;
			case 52:
				module->clk1_Divider_Mapped = 10.f;
				text = string::f("x 10 . 0");
				break;
			case 53:
				module->clk1_Divider_Mapped = 12.f;
				text = string::f("x 12 . 0");
				break;
			case 54:
				module->clk1_Divider_Mapped = 16.f;
				text = string::f("x 16 . 0");
				break;
			case 55:
				module->clk1_Divider_Mapped = 20.f;
				text = string::f("x 20 . 0");
				break;
			case 56:
				module->clk1_Divider_Mapped = 24.f;
				text = string::f("x 24 . 0");
				break;
			case 57:
				module->clk1_Divider_Mapped = 28.f;
				text = string::f("x 28 . 0");
				break;
			case 58:
				module->clk1_Divider_Mapped = 32.f;
				text = string::f("x 32 . 0");
				break;
			case 59:
				module->clk1_Divider_Mapped = 36.f;
				text = string::f("x 36 . 0");
				break;
			case 60:
				module->clk1_Divider_Mapped = 40.f;
				text = string::f("x 40 . 0");
				break;
			case 61:
				module->clk1_Divider_Mapped = 44.f;
				text = string::f("x 44 . 0");
				break;
			case 62:
				module->clk1_Divider_Mapped = 48.f;
				text = string::f("x 48 . 0");
				break;
			case 63:
				module->clk1_Divider_Mapped = 52.f;
				text = string::f("x 52 . 0");
				break;
			case 64:
				module->clk1_Divider_Mapped = 56.f;
				text = string::f("x 56 . 0");
				break;
			case 65:
				module->clk1_Divider_Mapped = 60.f;
				text = string::f("x 60 . 0");
				break;
			case 66:
				module->clk1_Divider_Mapped = 64.f;
				text = string::f("x 64 . 0");
				break;
			case 67:
				module->clk1_Divider_Mapped = 68.f;
				text = string::f("x 68 . 0");
				break;
			case 68:
				module->clk1_Divider_Mapped = 72.f;
				text = string::f("x 72 . 0");
				break;
			case 69:
				module->clk1_Divider_Mapped = 76.f;
				text = string::f("x 76 . 0");
				break;
			case 70:
				module->clk1_Divider_Mapped = 80.f;
				text = string::f("x 80 . 0");
				break;
			case 71:
				module->clk1_Divider_Mapped = 84.f;
				text = string::f("x 84 . 0");
				break;
			case 72:
				module->clk1_Divider_Mapped = 88.f;
				text = string::f("x 88 . 0");
				break;
			case 73:
				module->clk1_Divider_Mapped = 92.f;
				text = string::f("x 92 . 0");
				break;
			case 74:
				module->clk1_Divider_Mapped = 96.f;
				text = string::f("x 96 . 0");
				break;

			default:
				module->clk1_Divider_Mapped = 1.f;
				text = string::f("x 1 . 0");
				break;
			}
		}
		else
			text = string::f("x 2 . 0");
	}
};

struct TickerWidget : ModuleWidget
{
	TickerWidget(Ticker *module)
	{
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Ticker.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		// Clock Master Inputs
		// BPM
		Ticker_BPM_Display *Msr_BMP_display = createWidget<Ticker_BPM_Display>(mm2px(Vec(5.0, 13.5)));
		Msr_BMP_display->box.size = mm2px(Vec(16.0, 8.0));
		Msr_BMP_display->module = module;
		addChild(Msr_BMP_display);
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(29.0, 16.25)), module, Ticker::MSR_BPM_PARAM));
		addChild(createLightCentered<SmallSimpleLight<STSYellowLight>>(mm2px(Vec(29.0, 16.25)), module, Ticker::MSR_PULSE_LIGHT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(29.0, 26.75)), module, Ticker::MSR_BPM_IN_INPUT));

		// Reset
		addParam(createLightParamCentered<VCVLightButton<MediumSimpleLight<STSBlueLight>>>(mm2px(Vec(44.0, 16.25)), module, Ticker::MSR_RESET_BTN_PARAM, Ticker::MSR_RESET_LIGHT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(44.0, 26.75)), module, Ticker::MSR_RESET_IN_INPUT));

		// Run
		addParam(createLightParamCentered<VCVLightButton<MediumSimpleLight<STSBlueLight>>>(mm2px(Vec(59.0, 16.25)), module, Ticker::MSR_RUN_BTN_PARAM, Ticker::MSR_RUN_LIGHT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(59.0, 26.75)), module, Ticker::MSR_RUN_IN_INPUT));

		// Gate Length
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(74.0, 16.25)), module, Ticker::MSR_GATE_LEN_PARAM));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(74.0, 26.75)), module, Ticker::MSR_GATE_LEN_IN_INPUT));

		// Clock Master Outputs
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(92.1, 16.25)), module, Ticker::MSR_GATE_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(101.2, 16.25)), module, Ticker::MSR_TRIGGER_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(110.3, 16.25)), module, Ticker::MSR_RESET_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(119.4, 16.25)), module, Ticker::MSR_RUN_OUTPUT));

		// Clock 1 Panel
		// Divider
		Ticker_CLK1_Div_Display *CLK1_Div_display = createWidget<Ticker_CLK1_Div_Display>(mm2px(Vec(5.0, 60.0)));
		CLK1_Div_display->box.size = mm2px(Vec(19.0, 8.0));
		CLK1_Div_display->module = module;
		addChild(CLK1_Div_display);
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(29.5, 63.5)), module, Ticker::CLK1_DIV_PARAM));
		addChild(createLightCentered<SmallSimpleLight<STSYellowLight>>(mm2px(Vec(29.5, 63.5)), module, Ticker::CLK1_PULSE_LIGHT));

		// Phase shift
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(40.0, 63.5)), module, Ticker::CLK1_PHASE_PARAM));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(49.5, 63.5)), module, Ticker::CLK1_PHASE_IN_INPUT));

		// Gate Length
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(62.5, 63.5)), module, Ticker::CLK1_GATE_LEN_PARAM));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(72.0, 63.5)), module, Ticker::CLK1_GATE_LEN_IN_INPUT));

		// Swing Amount
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(85.0, 63.5)), module, Ticker::CLK1_SWING_PARAM));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(94.5, 63.5)), module, Ticker::CLK1_SWING_IN_INPUT));

		// Outputs
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(110.0, 63.5)), module, Ticker::CLK1_GATE_OUTPUT));
	}
};

Model *modelTicker = createModel<Ticker, TickerWidget>("Ticker");