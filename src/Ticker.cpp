#include "plugin.hpp"
#include "sts-base.hpp"
#include "Ticker.hpp"
#include <math.h>
#include <format>
struct Ticker : Module
{
	enum ParamId
	{
		MSR_BPM_PARAM,
		MSR_RESET_BTN_PARAM,
		MSR_RUN_BTN_PARAM,
		MSR_GATE_LEN_PARAM,
		ENUMS(CLK_DIV_PARAMS, 4),
		ENUMS(CLK_PHASE_PARAMS, 4),
		ENUMS(CLK_GATE_PARAMS, 4),
		ENUMS(CLK_SWING_PARAMS, 4),
		PARAMS_LEN
	};
	enum InputId
	{
		MSR_BPM_IN_INPUT,
		MSR_RESET_IN_INPUT,
		MSR_RUN_IN_INPUT,
		MSR_GATE_LEN_IN_INPUT,
		ENUMS(CLK_PHASE_INPUTS, 4),
		ENUMS(CLK_GATE_INPUTS, 4),
		ENUMS(CLK_SWING_INPUTS, 4),
		INPUTS_LEN
	};
	enum OutputId
	{
		MSR_GATE_OUTPUT,
		MSR_TRIGGER_OUTPUT,
		MSR_RESET_OUTPUT,
		MSR_RUN_OUTPUT,
		ENUMS(CLK_GATE_OUTPUTS, 4),
		ENUMS(CLK_TRIGGER_OUTPUTS, 4),
		OUTPUTS_LEN
	};
	enum LightId
	{
		MSR_RESET_LIGHT,
		MSR_RUN_LIGHT,
		MSR_PULSE_LIGHT,
		ENUMS(CLK_PULSE_LIGHTS, 4),
		LIGHTS_LEN
	};

	// Some class-wide parameters

	const float MIN_BPM_PARAM = 10.f;										 // minimum BPM value
	const float MAX_BPM_PARAM = 400.f;										 // maximum BPM value
	const float MIN_GATE_LEN = 5.f;											 // minimum gate lenght as % of a cycle
	const float MAX_GATE_LEN = 95.f;										 // maximum gate lenght as % of a cycle
	const float SWING_PHASE_TO_WAIT = (MAX_GATE_LEN - MIN_GATE_LEN) / 100.f; // wait for phase to reach this before computing next swing amount
	const float MAX_SWING_AMOUNT = 5.f;										 // maximum swing about as % of a cycle

	// Triggers to detect an button push on run or reset signal
	dsp::BooleanTrigger runButtonTrigger;
	dsp::BooleanTrigger resetButtonTrigger;

	// Triggers to detect an external run or reset signal
	dsp::SchmittTrigger runTrigger;
	dsp::SchmittTrigger resetTrigger;

	// Pulses for Gates and Triggers
	dsp::PulseGenerator runPulse;
	dsp::PulseGenerator resetPulse;
	dsp::PulseGenerator msr_GatePulse;
	dsp::PulseGenerator msr_TrgPulse;
	dsp::PulseGenerator clk_GatePulses[4];
	dsp::PulseGenerator clk_TrgPulses[4];

	bool is_Running = false; // is the clock running?

	bool runButtonTriggered = false;
	bool runTriggered = false;
	bool resetButtonTriggered = false;
	bool resetTriggered = false;

	// Vars to trigger Run, Reset and Clock Trigger outputs
	bool resetPulseState = false;
	bool runPulseState = false;
	bool msr_TrgState = false;
	bool msr_GateState = false;
	bool clk_TrgStates[4] = {false, false, false, false};
	bool clk_GateStates[4] = {false, false, false, false};

	const float One_Hz = 1.f / 60.f; // Factor to convert BPM to Hertz

	int msr_BPM = 120;			   // current Master BPM value param
	int msr_BPM_Old = 120;		   // Old Master BPM value
	bool msr_Gate_Started = false; // Toggle to see if we started a new master pulse

	float msr_Freq = 2.f;		   // current Master Frequency = BPM / 60
	float msr_Phase = 0.f;		   // holds the phase of the Master Clock
	float msr_Gate_Len = 50.f;	   // Master Gate length in %
	float msr_Gate_Duration = 0.f; // Master Gate duration
	float msr_Gate_Voltage;		   // Output voltage for Master Gate

	// Variable arrays for the 4 clocks
	int clk_Dividers[4] = {36, 36, 36, 36};					  // current Clock Divider params
	int clk_Dividers_Old[4] = {36, 36, 36, 36};				  // Old Clock Divider params
	bool clk_Gates_Started[4] = {false, false, false, false}; // Toggle to see if we started a new clock gate
	float clk_Dividers_Mapped[4] = {1.f, 1.f, 1.f, 1.f};	  // current clock Divider params mapped to actual divider factor
	float clk_Frequencies[4] = {2.f, 2.f, 2.f, 2.f};		  // current clock Frequencies
	float clk_Phases[4] = {0.f, 0.f, 0.f, 0.f};				  // holds the phases of the clocks
	float clk_Gates_Len[4] = {50.f, 50.f, 50.f, 50.f};		  // Clock gates length in %
	float clk_Gates_Duration[4] = {0.f, 0.f, 0.f, 0.f};		  // Clock gate durations
	float clk_Phases_Shift[4] = {0.f, 0.f, 0.f, 0.f};		  // Phase shift / delay of the pulse
	float clk_Swing_Amounts[4] = {0.f, 0.f, 0.f, 0.f};		  // Amount of Swing to apply
	float clk_Swing_Values[4] = {0.f, 0.f, 0.f, 0.f};		  // Amount of swing to apply
	float clk_Gates_Voltage[4] = {0.f, 0.f, 0.f, 0.f};		  // Output voltage for clock gates

	// A clock is basically a pulse, so using a modified part of the Pulse_VCO code here
	// phase_shift is used to delay/swing, swing_value is a small (<5%) phase shift on top of the phase modulation, pulse_width is a %%
	float STS_My_Pulse(float phase, float phase_shift, float swing_value, float pulse_width)
	{
		float local_phase;

		// Displace by phase shift, map into 0..1. Note that phase_shift can be negative in this code
		local_phase = (phase + phase_shift + swing_value) * 100.f;
		if (local_phase < 0.f)
			local_phase += 100.f;
		if (local_phase > 100.f)
			local_phase -= 100.f;

		if (local_phase < pulse_width)
			return 10.f;
		else
			return 0.f;
	}

	// DEBUG version of STS_My_Pulse
	float STS_My_Pulse_DBG(const char *txt, float phase, float phase_shift, float swing_value, float pulse_width)
	{
		float local_phase, ret_val;

		// Displace by phase shift, map into 0..1. Note that phase_shift can be negative in this code
		local_phase = (phase + phase_shift + swing_value) * 100.f;
		if (local_phase < 0.f)
			local_phase += 100.f;
		if (local_phase > 100.f)
			local_phase -= 100.f;

		if (local_phase < pulse_width)
			ret_val = 10.f;
		else
			ret_val = 0.f;
		INFO("%s: RET = %f , LOCAL_PHASE = %f, phase=%f, phase_shift = %f, swing_value = %f, pulse_width = %f", txt, ret_val, local_phase, phase, phase_shift, swing_value, pulse_width);
		return ret_val;
	}

	// This is an Initialize, not a RESET button push
	void onReset() override
	{
		int i = 0; // index for clock loops

		// Set defaults
		msr_BPM = msr_BPM_Old = 120;
		clk_Dividers[0] = clk_Dividers[1] = clk_Dividers[2] = clk_Dividers[3] = 36;
		clk_Dividers_Old[0] = clk_Dividers_Old[1] = clk_Dividers_Old[2] = clk_Dividers_Old[3] = 36;
		clk_Dividers_Mapped[0] = clk_Dividers_Mapped[1] = clk_Dividers_Mapped[2] = clk_Dividers_Mapped[3] = 36;

		is_Running = false;
		msr_Phase = 0.f;

		clk_Phases[0] = clk_Phases[1] = clk_Phases[2] = clk_Phases[3] = 0.f;
		clk_Phases_Shift[0] = clk_Phases_Shift[1] = clk_Phases_Shift[2] = clk_Phases_Shift[3] = 0.f;
		clk_Swing_Amounts[0] = clk_Swing_Amounts[1] = clk_Swing_Amounts[2] = clk_Swing_Amounts[3] = 0.f;
		clk_Swing_Values[0] = clk_Swing_Values[1] = clk_Swing_Values[2] = clk_Swing_Values[3] = 0.f;
		clk_Gates_Started[0] = clk_Gates_Started[1] = clk_Gates_Started[2] = clk_Gates_Started[3] = false;

		// Reset some other vars
		msr_TrgState = false;
		msr_GateState = false;
		clk_TrgStates[0] = clk_TrgStates[1] = clk_TrgStates[2] = clk_TrgStates[3] = false;
		clk_GateStates[0] = clk_GateStates[1] = clk_GateStates[2] = clk_GateStates[3] = false;
		runPulseState = false;
		resetPulseState = false;
		msr_Gate_Started = false;

		// Reset all triggers and gates
		resetPulse.reset();
		runPulse.reset();
		msr_GatePulse.reset();
		msr_TrgPulse.reset();
		for (i = 0; i < 4; i++)
		{
			clk_GatePulses[i].reset();
			clk_TrgPulses[i].reset();
		}

		// Reset all outputs & lights
		getOutput(MSR_GATE_OUTPUT).setVoltage(0.f);
		getOutput(MSR_TRIGGER_OUTPUT).setVoltage(0.f);
		getOutput(MSR_RESET_OUTPUT).setVoltage(0.f);
		getOutput(MSR_RUN_OUTPUT).setVoltage(0.f);

		getLight(MSR_RESET_LIGHT).setBrightness(0.f);
		getLight(MSR_RUN_LIGHT).setBrightness(0.f);
		getLight(MSR_PULSE_LIGHT).setBrightness(0.f);
		getLight(MSR_PULSE_LIGHT).setBrightness(0.f);
	}

	Ticker()
	{
		int i = 0;		 // loop index for all clocks
		std::string fmt; // string to format text
		char name[64];	 // string to format text

		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

		// Master Clock Params
		configParam(MSR_BPM_PARAM, MIN_BPM_PARAM, MAX_BPM_PARAM, 120.f, "BPM");
		configParam(MSR_GATE_LEN_PARAM, MIN_GATE_LEN, MAX_GATE_LEN, 50.f, "Gate Length", "%");
		// Master Clock Buttons
		configButton(MSR_RESET_BTN_PARAM, "Reset");
		configButton(MSR_RUN_BTN_PARAM, "Run");
		// Master Clock Inputs
		configInput(MSR_BPM_IN_INPUT, "BPM Voltage (0..10V)");
		configInput(MSR_RESET_IN_INPUT, "Reset Trigger");
		configInput(MSR_RUN_IN_INPUT, "Run Trigger");
		configInput(MSR_GATE_LEN_IN_INPUT, "Gate Length Voltage (0..10V)");
		// Master Clock Outputs
		configOutput(MSR_GATE_OUTPUT, "Master Clock Gate");
		configOutput(MSR_TRIGGER_OUTPUT, "Master Clock Trigger");
		configOutput(MSR_RESET_OUTPUT, "Reset Out");
		configOutput(MSR_RUN_OUTPUT, "Run Out");

		// Clock Params
		for (i = 0; i < 4; i++)
		{
			configParam(CLK_DIV_PARAMS + i, 0.f, 72.f, 36.f, "Divider", "");
			getParamQuantity(CLK_DIV_PARAMS + i)->snapEnabled = true;
			configParam(CLK_PHASE_PARAMS + i, -0.5f, 0.5f, 0.f, "Phase shift", " Cycle");
			configParam(CLK_GATE_PARAMS + i, MIN_GATE_LEN, MAX_GATE_LEN, 50.f, "Gate Length", "%");
			configParam(CLK_SWING_PARAMS + i, 0.f, MAX_SWING_AMOUNT, 0.f, "Swing Amount", "%");
		}

		// Clock Inputs
		for (i = 0; i < 4; i++)
		{
			(void)sprintf(name, "Clock %d Phase CV (0..10V)", i + 1);
			fmt = name;
			configInput(CLK_PHASE_INPUTS + i, fmt);
			(void)sprintf(name, "Clock %d Gate Lenth CV (0..10V)", i + 1);
			fmt = name;
			configInput(CLK_GATE_INPUTS + i, fmt);
			(void)sprintf(name, "Clock %d Swing Amount CV (0..10V)", i + 1);
			fmt = name;
			configInput(CLK_SWING_INPUTS + i, fmt);
		}

		// Clock Outputs
		for (i = 0; i < 4; i++)
		{

			(void)sprintf(name, "Clock %d Gate", i + 1);
			fmt = name;
			configOutput(CLK_GATE_OUTPUTS + i, fmt);
			(void)sprintf(name, "Clock %d Trigger", i + 1);
			fmt = name;
			configOutput(CLK_TRIGGER_OUTPUTS + i, fmt);
		}

		onReset();
	}

	void process(const ProcessArgs &args) override
	{
		int i = 0; // loop index for all clocks

		// Get all the values from the module UI

		// Master Clock
		// BPM Data = 10V mapped to range 10-400 BPM
		if (getInput(MSR_BPM_IN_INPUT).isConnected())
		{
			msr_BPM = MIN_BPM_PARAM + (MAX_BPM_PARAM - MIN_BPM_PARAM) * getInput(MSR_BPM_IN_INPUT).getVoltage() * 0.1f;
		}
		else
			msr_BPM = (int)getParam(MSR_BPM_PARAM).getValue();
		// Did we change the BPM? If so, reset all clock phases
		if (msr_BPM != msr_BPM_Old)
		{
			msr_BPM_Old = msr_BPM;
			msr_Phase = 0.f;
			clk_Phases[0] = clk_Phases[1] = clk_Phases[2] = clk_Phases[3] = 0.f;
		}
		// Gate Length Data = 10V mapped to range MIN/MAX_GATE_LEN%
		if (getInput(MSR_GATE_LEN_IN_INPUT).isConnected())
			msr_Gate_Len = MIN_GATE_LEN + (MAX_GATE_LEN - MIN_GATE_LEN) * getInput(MSR_GATE_LEN_IN_INPUT).getVoltage() * 0.1f;
		else
			msr_Gate_Len = (int)getParam(MSR_GATE_LEN_PARAM).getValue();

		// Now loop through the four clocks
		for (i = 0; i < 4; i++)
		{
			clk_Dividers[i] = getParam(CLK_DIV_PARAMS + i).getValue();
			clk_Dividers_Mapped[i] = div_to_factor[clk_Dividers[i]];

			// Did we change the Divider setting? If so, reset the phase as per the master clock and recompute the factor
			if (clk_Dividers[i] != clk_Dividers_Old[i])
			{
				clk_Dividers_Old[i] = clk_Dividers[i];
				msr_Phase = 0.f;
				clk_Phases[i] = clk_Phases[1] = clk_Phases[2] = clk_Phases[3] = 0.f;
			}

			// Phase data, 0..10V mapped to 0..1
			if (getInput(CLK_PHASE_INPUTS + i).isConnected())
				clk_Phases_Shift[i] = getInput(CLK_PHASE_INPUTS + i).getVoltage() * 0.1f;
			else
				clk_Phases_Shift[i] = getParam(CLK_PHASE_PARAMS + i).getValue();

			// Gate Length Data = 10V mapped to range 5-95%
			if (getInput(CLK_GATE_INPUTS + i).isConnected())
				clk_Gates_Len[i] = MIN_GATE_LEN + (MAX_GATE_LEN - MIN_GATE_LEN) * getInput(CLK_GATE_INPUTS + i).getVoltage() * 0.1f;
			else
				clk_Gates_Len[i] = (int)getParam(CLK_GATE_PARAMS + i).getValue();

			// Swing Amount Data = 10V mapped to a 5% range
			if (getInput(CLK_SWING_INPUTS + i).isConnected())
				clk_Swing_Amounts[i] = getInput(CLK_SWING_INPUTS + i).getVoltage() * 0.5f;
			else
				clk_Swing_Amounts[i] = getParam(CLK_SWING_PARAMS + i).getValue();
		}

		// PROCESSING OF ALL INPUT STARTS HERE
		//
		// Compute Master Clock Frequency and derive the individual clocks
		msr_Freq = msr_BPM * One_Hz;
		for (i = 0; i < 4; i++)
			clk_Frequencies[i] = msr_Freq * clk_Dividers_Mapped[i];

		// Was Run pressed or a pulse received on Run In?
		runButtonTriggered = runButtonTrigger.process(getParam(MSR_RUN_BTN_PARAM).getValue());
		runTriggered = runTrigger.process(getInput(MSR_RUN_IN_INPUT).getVoltage(), 0.1f, 2.f);

		if (runButtonTriggered || runTriggered)
		{
			// Start the run trigger
			runPulse.trigger(1e-3f);
			is_Running ^= true;
		}
		// Check if the run pulse should be sent, and if so, send it out
		runPulseState = runPulse.process(args.sampleTime);
		getOutput(MSR_RUN_OUTPUT).setVoltage((runPulseState) ? 10.f : 0.f);

		// Was Reset pressed or a pulse received on Reset In?
		resetButtonTriggered = resetButtonTrigger.process(getParam(MSR_RESET_BTN_PARAM).getValue());
		resetTriggered = resetTrigger.process(getInput(MSR_RESET_IN_INPUT).getVoltage(), 0.1f, 2.f);

		if (resetButtonTriggered || resetTriggered)
		{
			// Start the reset trigger
			resetPulse.trigger(1e-3f);

			// Reset some vars
			is_Running = false;
			msr_Phase = 0.f;
			clk_Phases[0] = clk_Phases[1] = clk_Phases[2] = clk_Phases[3] = 0.f;
		}
		// Check if the reset pulse should be sent, and if so, send it out
		resetPulseState = resetPulse.process(args.sampleTime);
		getOutput(MSR_RESET_OUTPUT).setVoltage((resetPulseState) ? 10.f : 0.f);

		// Toggle the Reset light. The smaller the delta time, the slower the fade
		getLight(MSR_RESET_LIGHT).setBrightnessSmooth(resetPulseState, 0.25f * args.sampleTime);

		if (is_Running)
		{
			// Accumulate the phase for each clock, make sure it rotates between 0.0 and 1.0
			msr_Phase += msr_Freq * args.sampleTime;
			if (msr_Phase >= 1.f)
				// msr_Phase -= 1.f;
				msr_Phase = 0.f;
			for (i = 0; i < 4; i++)
			{
				clk_Phases[i] += clk_Frequencies[i] * args.sampleTime;
				if (clk_Phases[i] >= 1.f)
					// clk_Phases[i] -= 1.f;
					clk_Phases[i] = 0.f;
			}

			// Master Clock
			// Compute the duration of the master gate
			msr_Gate_Duration = msr_Gate_Len / (msr_Freq * 100.f);
			// compute the Master Clock signal, which has no swing/phase shift
			msr_Gate_Voltage = STS_My_Pulse(msr_Phase, 0.f, 0.f, msr_Gate_Len);
			// Start the master clock trigger, if the new pulse started
			if (msr_Gate_Voltage > 0.f && !msr_Gate_Started)
			{
				msr_Gate_Started = true; // We start a new Master Gate
				// Now trigger the pulses
				msr_TrgPulse.trigger(1e-3f);
				msr_GatePulse.trigger(msr_Gate_Duration);
			}
			if (msr_Gate_Voltage == 0.f)
				msr_Gate_Started = false;

			// Check the master trigger & gate pulse
			msr_TrgState = msr_TrgPulse.process(args.sampleTime);
			msr_GateState = msr_GatePulse.process(args.sampleTime);

			// Output the master gate & trigger & lights
			getOutput(MSR_GATE_OUTPUT).setVoltage((msr_GateState) ? 10.f : 0.f);
			getOutput(MSR_TRIGGER_OUTPUT).setVoltage((msr_TrgState) ? 10.f : 0.f);
			getLight(MSR_PULSE_LIGHT).setBrightnessSmooth(msr_Gate_Voltage > 0.f, args.sampleTime);
			getLight(MSR_RUN_LIGHT).setBrightness(1.f);

			// Run through all four clocks
			for (i = 0; i < 4; i++)
			{
				// Compute the duration of the clock gate
				clk_Gates_Duration[i] = clk_Gates_Len[i] / (clk_Frequencies[i] * 100.f);

				// Compute the derived clocks as per the pulse width, phase and random swing amount
				if (i != 0)
					clk_Gates_Voltage[i] = STS_My_Pulse(clk_Phases[i], clk_Phases_Shift[i], clk_Swing_Values[i], clk_Gates_Len[i]);
				else
					clk_Gates_Voltage[i] = STS_My_Pulse_DBG("CLK1 DBG: ", clk_Phases[i], clk_Phases_Shift[i], clk_Swing_Values[i], clk_Gates_Len[i]);
					
				// Are we outputting a new clock gate and not waiting for the previous one?
				if (clk_Gates_Voltage[i] > 0.0f && !clk_Gates_Started[i])
				{
					clk_Gates_Started[i] = true;
					clk_TrgPulses[i].trigger(1e-3f);
					clk_GatePulses[i].trigger(clk_Gates_Duration[i]);
				}

				// Check the clock trigger & gate pulse
				clk_TrgStates[i] = clk_TrgPulses[i].process(args.sampleTime);
				clk_GateStates[i] = clk_GatePulses[i].process(args.sampleTime);

				// If gate is running (gate pulse itself should be over) & wait finished and swing amount > 0, recompute swing
				if (clk_Gates_Started[i] && (clk_Phases[i] > SWING_PHASE_TO_WAIT) && !clk_GateStates[i])
				{
					// Recompute a new swing value as part of a cycle
					clk_Swing_Values[i] = (1.f - 2.f * rack::random::uniform()) * clk_Swing_Amounts[i] * 0.01;
					clk_Gates_Started[i] = false;
				}

				// Output the CLK1 gate & trigger & lights
				getOutput(CLK_GATE_OUTPUTS + i).setVoltage((clk_GateStates[i]) ? 10.f : 0.f);
				getOutput(CLK_TRIGGER_OUTPUTS + i).setVoltage((clk_TrgStates[i]) ? 10.f : 0.f);
				getLight(CLK_PULSE_LIGHTS + i).setBrightnessSmooth(clk_Gates_Voltage[i] > 0.f, args.sampleTime);
			}
		}
		else // Not running
		{
			// Set phase to 0.0 to avoid the first pulse to be too short/quick
			msr_Phase = 0.f;
			msr_Gate_Started = false;
			clk_Phases[0] = clk_Phases[1] = clk_Phases[2] = clk_Phases[3] = 0.f;
			clk_Gates_Started[0] = clk_Gates_Started[1] = clk_Gates_Started[2] = clk_Gates_Started[3] = false;

			// Reset all outputs & lights
			getOutput(MSR_GATE_OUTPUT).setVoltage(0.f);
			for (i = 0; i < 4; i++)
			{
				getOutput(CLK_GATE_OUTPUTS + i).setVoltage(0.f);
				getLight(CLK_PULSE_LIGHTS + i).setBrightness(0.f);
			}

			getLight(MSR_RUN_LIGHT).setBrightness(0.f);
			getLight(MSR_PULSE_LIGHT).setBrightness(0.f);
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
			display_BPM = module->msr_BPM;
		text = string::f("%0*d", 3, display_BPM);
	}
};

// Clock 1..4 Display structure. This should be refactored in a much easier way...for the future...

struct Ticker_CLK1_Div_Display : CLK_Div_Display
{
	Ticker *module;

	// Icky code to map the param steps to specific divider or multiplier intervals
	void step() override
	{
		int clk_Divider = 38;
		if (module)
			clk_Divider = module->clk_Dividers[0];
		text = div_to_text[clk_Divider];
	}
};

struct Ticker_CLK2_Div_Display : CLK_Div_Display
{
	Ticker *module;

	// Icky code to map the param steps to specific divider or multiplier intervals
	void step() override
	{
		int clk_Divider = 38;
		if (module)
			clk_Divider = module->clk_Dividers[1];
		text = div_to_text[clk_Divider];
	}
};

struct Ticker_CLK3_Div_Display : CLK_Div_Display
{
	Ticker *module;

	// Icky code to map the param steps to specific divider or multiplier intervals
	void step() override
	{
		int clk_Divider = 38;
		if (module)
			clk_Divider = module->clk_Dividers[2];
		text = div_to_text[clk_Divider];
	}
};

struct Ticker_CLK4_Div_Display : CLK_Div_Display
{
	Ticker *module;

	// Icky code to map the param steps to specific divider or multiplier intervals
	void step() override
	{
		int clk_Divider = 38;
		if (module)
			clk_Divider = module->clk_Dividers[3];
		text = div_to_text[clk_Divider];
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
		Ticker_CLK1_Div_Display *CLK1_Div_display = createWidget<Ticker_CLK1_Div_Display>(mm2px(Vec(5.0, 43.0)));

		CLK1_Div_display->box.size = mm2px(Vec(19.0, 8.0));
		CLK1_Div_display->module = module;
		addChild(CLK1_Div_display);
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(29.5, 46.5)), module, Ticker::CLK_DIV_PARAMS + 0));
		addChild(createLightCentered<SmallSimpleLight<STSYellowLight>>(mm2px(Vec(29.5, 46.5)), module, Ticker::CLK_PULSE_LIGHTS + 0));

		// Phase shift
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(40.0, 46.5)), module, Ticker::CLK_PHASE_PARAMS + 0));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(49.5, 46.5)), module, Ticker::CLK_PHASE_INPUTS + 0));

		// Gate Length
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(62.5, 46.5)), module, Ticker::CLK_GATE_PARAMS + 0));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(72.0, 46.5)), module, Ticker::CLK_GATE_INPUTS + 0));

		// Swing Amount
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(85.0, 46.5)), module, Ticker::CLK_SWING_PARAMS + 0));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(94.5, 46.5)), module, Ticker::CLK_SWING_INPUTS + 0));

		// Outputs
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(108.0, 46.5)), module, Ticker::CLK_GATE_OUTPUTS + 0));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(119.0, 46.5)), module, Ticker::CLK_TRIGGER_OUTPUTS + 0));

		// Clock 2 Panel
		// Divider
		Ticker_CLK2_Div_Display *CLK2_Div_display = createWidget<Ticker_CLK2_Div_Display>(mm2px(Vec(5.0, 60.0)));
		CLK2_Div_display->box.size = mm2px(Vec(19.0, 8.0));
		CLK2_Div_display->module = module;
		addChild(CLK2_Div_display);
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(29.5, 63.5)), module, Ticker::CLK_DIV_PARAMS + 1));
		addChild(createLightCentered<SmallSimpleLight<STSYellowLight>>(mm2px(Vec(29.5, 63.5)), module, Ticker::CLK_PULSE_LIGHTS + 1));

		// Phase shift
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(40.0, 63.5)), module, Ticker::CLK_PHASE_PARAMS + 1));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(49.5, 63.5)), module, Ticker::CLK_PHASE_INPUTS + 1));

		// Gate Length
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(62.5, 63.5)), module, Ticker::CLK_GATE_PARAMS + 1));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(72.0, 63.5)), module, Ticker::CLK_GATE_INPUTS + 1));

		// Swing Amount
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(85.0, 63.5)), module, Ticker::CLK_SWING_PARAMS + 1));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(94.5, 63.5)), module, Ticker::CLK_SWING_INPUTS + 1));

		// Outputs
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(108.0, 63.5)), module, Ticker::CLK_GATE_OUTPUTS + 1));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(119.0, 63.5)), module, Ticker::CLK_TRIGGER_OUTPUTS + 1));

		// Clock 3 Panel
		// Divider
		Ticker_CLK3_Div_Display *CLK3_Div_display = createWidget<Ticker_CLK3_Div_Display>(mm2px(Vec(5.0, 77.0)));
		CLK3_Div_display->box.size = mm2px(Vec(19.0, 8.0));
		CLK3_Div_display->module = module;
		addChild(CLK3_Div_display);
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(29.5, 80.5)), module, Ticker::CLK_DIV_PARAMS + 2));
		addChild(createLightCentered<SmallSimpleLight<STSYellowLight>>(mm2px(Vec(29.5, 80.5)), module, Ticker::CLK_PULSE_LIGHTS + 2));

		// Phase shift
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(40.0, 80.5)), module, Ticker::CLK_PHASE_PARAMS + 2));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(49.5, 80.5)), module, Ticker::CLK_PHASE_INPUTS + 2));

		// Gate Length
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(62.5, 80.5)), module, Ticker::CLK_GATE_PARAMS + 2));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(72.0, 80.5)), module, Ticker::CLK_GATE_INPUTS + 2));

		// Swing Amount
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(85.0, 80.5)), module, Ticker::CLK_SWING_PARAMS + 2));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(94.5, 80.5)), module, Ticker::CLK_SWING_INPUTS + 2));

		// Outputs
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(108.0, 80.5)), module, Ticker::CLK_GATE_OUTPUTS + 2));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(119.0, 80.5)), module, Ticker::CLK_TRIGGER_OUTPUTS + 2));

		// Clock 4 Panel
		// Divider
		Ticker_CLK4_Div_Display *CLK4_Div_display = createWidget<Ticker_CLK4_Div_Display>(mm2px(Vec(5.0, 94.0)));
		CLK4_Div_display->box.size = mm2px(Vec(19.0, 8.0));
		CLK4_Div_display->module = module;
		addChild(CLK4_Div_display);
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(29.5, 97.5)), module, Ticker::CLK_DIV_PARAMS + 3));
		addChild(createLightCentered<SmallSimpleLight<STSYellowLight>>(mm2px(Vec(29.5, 97.5)), module, Ticker::CLK_PULSE_LIGHTS + 3));

		// Phase shift
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(40.0, 97.5)), module, Ticker::CLK_PHASE_PARAMS + 3));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(49.5, 97.5)), module, Ticker::CLK_PHASE_INPUTS + 3));

		// Gate Length
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(62.5, 97.5)), module, Ticker::CLK_GATE_PARAMS + 3));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(72.0, 97.5)), module, Ticker::CLK_GATE_INPUTS + 3));

		// Swing Amount
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(85.0, 97.5)), module, Ticker::CLK_SWING_PARAMS + 3));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(94.5, 97.5)), module, Ticker::CLK_SWING_INPUTS + 3));

		// Outputs
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(108.0, 97.5)), module, Ticker::CLK_GATE_OUTPUTS + 3));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(119.0, 97.5)), module, Ticker::CLK_TRIGGER_OUTPUTS + 3));
	}
};

Model *modelTicker = createModel<Ticker, TickerWidget>("Ticker");