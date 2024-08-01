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
		MSR_GATE_PARAM,
		ENUMS(CLK_DIV_PARAMS, 4),
		ENUMS(CLK_PHASE_PARAMS, 4),
		ENUMS(CLK_GATE_PARAMS, 4),
		ENUMS(CLK_SWING_PARAMS, 4),
		PARAMS_LEN
	};
	enum InputId
	{
		MSR_BPM_INPUT,
		MSR_RESET_INPUT,
		MSR_RUN_INPUT,
		MSR_GATE_INPUT,
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
	dsp::PulseGenerator clk1_GatePulse;
	dsp::PulseGenerator clk1_TrgPulse;
	dsp::PulseGenerator clk2_GatePulse;
	dsp::PulseGenerator clk2_TrgPulse;
	dsp::PulseGenerator clk3_GatePulse;
	dsp::PulseGenerator clk3_TrgPulse;
	dsp::PulseGenerator clk4_GatePulse;
	dsp::PulseGenerator clk4_TrgPulse;

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
	bool clk1_TrgState = false;
	bool clk1_GateState = false;
	bool clk2_TrgState = false;
	bool clk2_GateState = false;
	bool clk3_TrgState = false;
	bool clk3_GateState = false;
	bool clk4_TrgState = false;
	bool clk4_GateState = false;

	const float One_Hz = 1.f / 60.f; // Factor to convert BPM to Hertz

	int msr_BPM = 120;			   // current Master BPM value param
	int msr_BPM_Old = 120;		   // Old Master BPM value
	bool msr_Gate_Started = false; // Toggle to see if we started a new master pulse

	float msr_Freq = 2.f;		   // current Master Frequency = BPM / 60
	float msr_Phase = 0.f;		   // holds the phase of the Master Clock
	float msr_Gate_Len = 50.f;	   // Master Gate length in %
	float msr_Gate_Duration = 0.f; // Master Gate duration
	float msr_Gate_Voltage;		   // Output voltage for Master Gate

	bool clk1_Gate_Started = false;	 // Toggle to see if we started a new CLK1 gate
	int clk1_Divider = 36;			 // current CLK1 Divider param
	int clk1_Divider_Old = 36;		 // Old CLK1 Divider
	float clk1_Divider_Mapped = 1.f; // current CLK1 Divider param mapped to actual divider factor
	float clk1_Freq = 2.f;			 // current CLK1 Frequency
	float clk1_Phase = 0.f;			 // holds the phase of CLK1
	float clk1_Gate_Len = 50.f;		 // CLK1 Gate length in %
	float clk1_Gate_Duration = 0.f;	 // CLK1 Gate duration
	float clk1_Phase_Shift = 0.f;	 // Phase shift / delay of the pulse
	float clk1_Swing_Amount = 0.f;	 // Amount of Swing to apply
	float clk1_Swing_value = 0.f;	 // Amount of swing to apply
	float clk1_Gate_Voltage = 0.f;	 // Output voltage for CLK1 Gate

	bool clk2_Gate_Started = false;	 // Toggle to see if we started a new CLK2 gate
	int clk2_Divider = 36;			 // current CLK2 Divider param
	int clk2_Divider_Old = 36;		 // Old CLK2 Divider
	float clk2_Divider_Mapped = 1.f; // current CLK2 Divider param mapped to actual divider factor
	float clk2_Freq = 2.f;			 // current CLK2 Frequency
	float clk2_Phase = 0.f;			 // holds the phase of CLK2
	float clk2_Gate_Len = 50.f;		 // CLK2 Gate length in %
	float clk2_Gate_Duration = 0.f;	 // CLK2 Gate duration
	float clk2_Phase_Shift = 0.f;	 // Phase shift / delay of the pulse
	float clk2_Swing_Amount = 0.f;	 // Amount of Swing to apply
	float clk2_Swing_value = 0.f;	 // Amount of swing to apply
	float clk2_Gate_Voltage = 0.f;	 // Output voltage for CLK2 Gate

	bool clk3_Gate_Started = false;	 // Toggle to see if we started a new CLK3 gate
	int clk3_Divider = 36;			 // current CLK3 Divider param
	int clk3_Divider_Old = 36;		 // Old CLK3 Divider
	float clk3_Divider_Mapped = 1.f; // current CLK3 Divider param mapped to actual divider factor
	float clk3_Freq = 2.f;			 // current CLK3 Frequency
	float clk3_Phase = 0.f;			 // holds the phase of CLK3
	float clk3_Gate_Len = 50.f;		 // CLK3 Gate length in %
	float clk3_Gate_Duration = 0.f;	 // CLK3 Gate duration
	float clk3_Phase_Shift = 0.f;	 // Phase shift / delay of the pulse
	float clk3_Swing_Amount = 0.f;	 // Amount of Swing to apply
	float clk3_Swing_value = 0.f;	 // Amount of swing to apply
	float clk3_Gate_Voltage = 0.f;	 // Output voltage for CLK3 Gate

	bool clk4_Gate_Started = false;	 // Toggle to see if we started a new CLK4 gate
	int clk4_Divider = 36;			 // current CLK4 Divider param
	int clk4_Divider_Old = 36;		 // Old CLK4 Divider
	float clk4_Divider_Mapped = 1.f; // current CLK4 Divider param mapped to actual divider factor
	float clk4_Freq = 2.f;			 // current CLK4 Frequency
	float clk4_Phase = 0.f;			 // holds the phase of CLK4
	float clk4_Gate_Len = 50.f;		 // CLK4 Gate length in %
	float clk4_Gate_Duration = 0.f;	 // CLK4 Gate duration
	float clk4_Phase_Shift = 0.f;	 // Phase shift / delay of the pulse
	float clk4_Swing_Amount = 0.f;	 // Amount of Swing to apply
	float clk4_Swing_value = 0.f;	 // Amount of swing to apply
	float clk4_Gate_Voltage = 0.f;	 // Output voltage for CLK4 Gate

	// A clock is basically a pulse, so using a modified part of the Pulse_VCO code here
	// Master pulse has no swing and phase_shift, pulse_width is a percentage
	float master_Pulse(float phase, float pulse_width)
	{
		float local_phase;

		// Displace by phase shift, map into 0..1. Note that phase_shift can be negative in this code
		local_phase = phase * 100.f;

		if (local_phase < pulse_width)
			return 10.f;
		else
			return 0.f;
	}

	// A clock is basically a pulse, so using a modified part of the Pulse_VCO code here
	// phase_shift is used to delay/swing, swing_value is a small (<5%) phase shift on top of the phase modulation, pulse_width is a %%
	float clock_Pulse(float phase, float phase_shift, float swing_value, float pulse_width)
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
		// Set defaults
		msr_BPM = msr_BPM_Old = 120;
		clk1_Divider = 36;
		clk1_Divider_Old = 36;
		clk2_Divider = 36;
		clk2_Divider_Old = 36;
		clk3_Divider = 36;
		clk3_Divider_Old = 36;
		clk4_Divider = 36;
		clk4_Divider_Old = 36;
		clk1_Divider_Mapped = 1.f;
		clk2_Divider_Mapped = 1.f;
		clk3_Divider_Mapped = 1.f;
		clk4_Divider_Mapped = 1.f;
		is_Running = false;
		msr_Phase = 0.f;
		clk1_Phase = 0.f;
		clk1_Phase_Shift = 0.f;
		clk1_Swing_Amount = 0.f;
		clk1_Swing_value = 0.f;
		clk2_Phase = 0.f;
		clk2_Phase_Shift = 0.f;
		clk2_Swing_Amount = 0.f;
		clk2_Swing_value = 0.f;
		clk3_Phase = 0.f;
		clk3_Phase_Shift = 0.f;
		clk3_Swing_Amount = 0.f;
		clk3_Swing_value = 0.f;
		clk4_Phase = 0.f;
		clk4_Phase_Shift = 0.f;
		clk4_Swing_Amount = 0.f;
		clk4_Swing_value = 0.f;

		// Reset some other vars
		msr_TrgState = false;
		msr_GateState = false;
		clk1_TrgState = false;
		clk1_GateState = false;
		clk2_TrgState = false;
		clk2_GateState = false;
		clk3_TrgState = false;
		clk3_GateState = false;
		clk4_TrgState = false;
		clk4_GateState = false;
		runPulseState = false;
		resetPulseState = false;
		msr_Gate_Started = false;
		clk1_Gate_Started = false;
		clk2_Gate_Started = false;
		clk3_Gate_Started = false;
		clk4_Gate_Started = false;

		// Reset all triggers and gates
		resetPulse.reset();
		runPulse.reset();
		msr_GatePulse.reset();
		msr_TrgPulse.reset();
		clk1_GatePulse.reset();
		clk1_TrgPulse.reset();
		clk2_GatePulse.reset();
		clk2_TrgPulse.reset();
		clk3_GatePulse.reset();
		clk3_TrgPulse.reset();
		clk4_GatePulse.reset();
		clk4_TrgPulse.reset();

		// Reset all outputs & lights
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
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

		// Master Clock Params
		configParam(MSR_BPM_PARAM, MIN_BPM_PARAM, MAX_BPM_PARAM, 120.f, "BPM");
		configParam(MSR_GATE_PARAM, MIN_GATE_LEN, MAX_GATE_LEN, 50.f, "Gate Length", "%");
		// Master Clock Buttons
		configButton(MSR_RESET_BTN_PARAM, "Reset");
		configButton(MSR_RUN_BTN_PARAM, "Run");
		// Master Clock Inputs
		configInput(MSR_BPM_INPUT, "BPM Voltage (0..10V)");
		configInput(MSR_RESET_INPUT, "Reset Trigger");
		configInput(MSR_RUN_INPUT, "Run Trigger");
		configInput(MSR_GATE_INPUT, "Gate Length Voltage (0..10V)");
		// Master Clock Outputs
		configOutput(MSR_GATE_OUTPUT, "Master Clock Gate");
		configOutput(MSR_TRIGGER_OUTPUT, "Master Clock Trigger");
		configOutput(MSR_RESET_OUTPUT, "Reset Out");
		configOutput(MSR_RUN_OUTPUT, "Run Out");

		// Clock 1 Params
		configParam(CLK_DIV_PARAMS + 0, 0.f, 72.f, 36.f, "Divider", "");
		getParamQuantity(CLK_DIV_PARAMS + 0)->snapEnabled = true;
		configParam(CLK_PHASE_PARAMS + 0, -0.5f, 0.5f, 0.f, "Phase Shift", " Cycle");
		configParam(CLK_GATE_PARAMS + 0, MIN_GATE_LEN, MAX_GATE_LEN, 50.f, "Gate Length", "%");
		configParam(CLK_SWING_PARAMS + 0, 0.f, MAX_SWING_AMOUNT, 0.f, "Swing Amount", "%");
		// Clock 1 Inputs
		configInput(CLK_PHASE_INPUTS + 0, "Phase Shift Voltage (0..10V)");
		configInput(CLK_GATE_INPUTS + 0, "Gate Length Voltage (0..10V)");
		configInput(CLK_SWING_INPUTS + 0, "Swing Amount Voltage (0..10V)");
		// Clock 1 Outputs
		configOutput(CLK_GATE_OUTPUTS + 0, "Clock 1 Gate");
		configOutput(CLK_TRIGGER_OUTPUTS + 0, "Clock 1 Trigger");

		// Clock 2 Params
		configParam(CLK_DIV_PARAMS + 1, 0.f, 72.f, 36.f, "Divider", "");
		getParamQuantity(CLK_DIV_PARAMS + 1)->snapEnabled = true;
		configParam(CLK_PHASE_PARAMS + 1, -0.5f, 0.5f, 0.f, "Phase Shift", " Cycle");
		configParam(CLK_GATE_PARAMS + 1, MIN_GATE_LEN, MAX_GATE_LEN, 50.f, "Gate Length", "%");
		configParam(CLK_SWING_PARAMS + 1, 0.f, MAX_SWING_AMOUNT, 0.f, "Swing Amount", "%");
		// Clock 2 Inputs
		configInput(CLK_PHASE_INPUTS + 1, "Phase Shift Voltage (0..10V)");
		configInput(CLK_GATE_INPUTS + 1, "Gate Length Voltage (0..10V)");
		configInput(CLK_SWING_INPUTS + 1, "Swing Amount Voltage (0..10V)");
		// Clock 2 Outputs
		configOutput(CLK_GATE_OUTPUTS + 1, "Clock 2 Gate");
		configOutput(CLK_TRIGGER_OUTPUTS + 1, "Clock 2 Trigger");

		// Clock 3 Params
		configParam(CLK_DIV_PARAMS + 2, 0.f, 72.f, 36.f, "Divider", "");
		getParamQuantity(CLK_DIV_PARAMS + 2)->snapEnabled = true;
		configParam(CLK_PHASE_PARAMS + 2, -0.5f, 0.5f, 0.f, "Phase Shift", " Cycle");
		configParam(CLK_GATE_PARAMS + 2, MIN_GATE_LEN, MAX_GATE_LEN, 50.f, "Gate Length", "%");
		configParam(CLK_SWING_PARAMS + 2, 0.f, MAX_SWING_AMOUNT, 0.f, "Swing Amount", "%");
		// Clock 3 Inputs
		configInput(CLK_PHASE_INPUTS + 2, "Phase Shift Voltage (0..10V)");
		configInput(CLK_GATE_INPUTS + 2, "Gate Length Voltage (0..10V)");
		configInput(CLK_SWING_INPUTS + 2, "Swing Amount Voltage (0..10V)");
		// Clock 3 Outputs
		configOutput(CLK_GATE_OUTPUTS + 2, "Clock 3 Gate");
		configOutput(CLK_TRIGGER_OUTPUTS + 2, "Clock 3 Trigger");

		// Clock 4 Params
		configParam(CLK_DIV_PARAMS + 3, 0.f, 72.f, 36.f, "Divider", "");
		getParamQuantity(CLK_DIV_PARAMS + 3)->snapEnabled = true;
		configParam(CLK_PHASE_PARAMS + 3, -0.5f, 0.5f, 0.f, "Phase Shift", " Cycle");
		configParam(CLK_GATE_PARAMS + 3, MIN_GATE_LEN, MAX_GATE_LEN, 50.f, "Gate Length", "%");
		configParam(CLK_SWING_PARAMS + 3, 0.f, MAX_SWING_AMOUNT, 0.f, "Swing Amount", "%");
		// Clock 4 Inputs
		configInput(CLK_PHASE_INPUTS + 3, "Phase Shift Voltage (0..10V)");
		configInput(CLK_GATE_INPUTS + 3, "Gate Length Voltage (0..10V)");
		configInput(CLK_SWING_INPUTS + 3, "Swing Amount Voltage (0..10V)");
		// Clock 4 Outputs
		configOutput(CLK_GATE_OUTPUTS + 3, "Clock 4 Gate");
		configOutput(CLK_TRIGGER_OUTPUTS + 3, "Clock 4 Trigger");

		onReset();
	}

	void process(const ProcessArgs &args) override
	{
		int i = 0; // index for clock loops

		// Get all the values from the module UI

		// Master Clock
		// BPM Data = 10V mapped to range 10-400 BPM
		if (getInput(MSR_BPM_INPUT).isConnected())
		{
			msr_BPM = MIN_BPM_PARAM + (MAX_BPM_PARAM - MIN_BPM_PARAM) * getInput(MSR_BPM_INPUT).getVoltage() * 0.1f;
		}
		else
			msr_BPM = (int)getParam(MSR_BPM_PARAM).getValue();
		// Did we change the BPM? If so, reset all clock phases
		if (msr_BPM != msr_BPM_Old)
		{
			msr_BPM_Old = msr_BPM;
			msr_Phase = 0.f;
			clk1_Phase = 0.f;
			clk2_Phase = 0.f;
			clk3_Phase = 0.f;
			clk4_Phase = 0.f;
		}
		// Gate Length Data = 10V mapped to range MIN/MAX_GATE_LEN%
		if (getInput(MSR_GATE_INPUT).isConnected())
			msr_Gate_Len = MIN_GATE_LEN + (MAX_GATE_LEN - MIN_GATE_LEN) * getInput(MSR_GATE_INPUT).getVoltage() * 0.1f;
		else
			msr_Gate_Len = (int)getParam(MSR_GATE_PARAM).getValue();

		// Clock 1
		// Divider data
		clk1_Divider = (int)getParam(CLK_DIV_PARAMS + 0).getValue();
		clk1_Divider_Mapped = div_to_factor[clk1_Divider];

		// Did we change the Divider setting? If so, reset the phase as per the master clock and recompute the factor
		if (clk1_Divider != clk1_Divider_Old)
		{
			clk1_Divider_Old = clk1_Divider;
			msr_Phase = 0.f;
			clk1_Phase = 0.f;
			clk2_Phase = 0.f;
			clk3_Phase = 0.f;
			clk4_Phase = 0.f;
		}
		// Phase data, 0..10V mapped to 0..1
		if (getInput(CLK_PHASE_INPUTS + 0).isConnected())
			clk1_Phase_Shift = getInput(CLK_PHASE_INPUTS + 0).getVoltage() * 0.1f;
		else
			clk1_Phase_Shift = getParam(CLK_PHASE_PARAMS + 0).getValue();

		// Gate Length Data = 10V mapped to range 5-95%
		if (getInput(CLK_GATE_INPUTS + 0).isConnected())
			clk1_Gate_Len = MIN_GATE_LEN + (MAX_GATE_LEN - MIN_GATE_LEN) * getInput(CLK_GATE_INPUTS + 0).getVoltage() * 0.1f;
		else
			clk1_Gate_Len = (int)getParam(CLK_GATE_PARAMS + 0).getValue();

		// Swing Amount Data = 10V mapped to a 5% range
		if (getInput(CLK_SWING_INPUTS + 0).isConnected())
			clk1_Swing_Amount = getInput(CLK_SWING_INPUTS + 0).getVoltage() * 0.5f;
		else
			clk1_Swing_Amount = (int)getParam(CLK_SWING_PARAMS + 0).getValue();

		// Clock 2
		// Divider data
		clk2_Divider = (int)getParam(CLK_DIV_PARAMS + 1).getValue();
		clk2_Divider_Mapped = div_to_factor[clk2_Divider];

		// Did we change the Divider setting? If so, reset the phase as per the master clock and recompute the factor
		if (clk2_Divider != clk2_Divider_Old)
		{
			clk2_Divider_Old = clk2_Divider;
			msr_Phase = 0.f;
			clk1_Phase = 0.f;
			clk2_Phase = 0.f;
			clk3_Phase = 0.f;
			clk4_Phase = 0.f;
		}
		// Phase data, 0..10V mapped to 0..1
		if (getInput(CLK_PHASE_INPUTS + 1).isConnected())
			clk2_Phase_Shift = getInput(CLK_PHASE_INPUTS + 1).getVoltage() * 0.1f;
		else
			clk2_Phase_Shift = getParam(CLK_PHASE_PARAMS + 1).getValue();

		// Gate Length Data = 10V mapped to range 5-95%
		if (getInput(CLK_GATE_INPUTS + 1).isConnected())
			clk2_Gate_Len = MIN_GATE_LEN + (MAX_GATE_LEN - MIN_GATE_LEN) * getInput(CLK_GATE_INPUTS + 1).getVoltage() * 0.1f;
		else
			clk2_Gate_Len = (int)getParam(CLK_GATE_PARAMS + 1).getValue();

		// Swing Amount Data = 10V mapped to a 5% range
		if (getInput(CLK_SWING_INPUTS + 1).isConnected())
			clk2_Swing_Amount = getInput(CLK_SWING_INPUTS + 1).getVoltage() * 0.5f;
		else
			clk2_Swing_Amount = (int)getParam(CLK_SWING_PARAMS + 1).getValue();

		// Clock 3
		// Divider data
		clk3_Divider = (int)getParam(CLK_DIV_PARAMS + 2).getValue();
		clk3_Divider_Mapped = div_to_factor[clk3_Divider];

		// Did we change the Divider setting? If so, reset the phase as per the master clock and recompute the factor
		if (clk3_Divider != clk3_Divider_Old)
		{
			clk3_Divider_Old = clk3_Divider;
			msr_Phase = 0.f;
			clk1_Phase = 0.f;
			clk2_Phase = 0.f;
			clk3_Phase = 0.f;
			clk4_Phase = 0.f;
		}
		// Phase data, 0..10V mapped to 0..1
		if (getInput(CLK_PHASE_INPUTS + 2).isConnected())
			clk3_Phase_Shift = getInput(CLK_PHASE_INPUTS + 2).getVoltage() * 0.1f;
		else
			clk3_Phase_Shift = getParam(CLK_PHASE_PARAMS + 2).getValue();

		// Gate Length Data = 10V mapped to range 5-95%
		if (getInput(CLK_GATE_INPUTS + 2).isConnected())
			clk3_Gate_Len = MIN_GATE_LEN + (MAX_GATE_LEN - MIN_GATE_LEN) * getInput(CLK_GATE_INPUTS + 2).getVoltage() * 0.1f;
		else
			clk3_Gate_Len = (int)getParam(CLK_GATE_PARAMS + 2).getValue();

		// Swing Amount Data = 10V mapped to a 5% range
		if (getInput(CLK_SWING_INPUTS + 2).isConnected())
			clk3_Swing_Amount = getInput(CLK_SWING_INPUTS + 2).getVoltage() * 0.5f;
		else
			clk3_Swing_Amount = (int)getParam(CLK_SWING_PARAMS + 2).getValue();

		// Clock 4
		// Divider data
		clk4_Divider = (int)getParam(CLK_DIV_PARAMS + 3).getValue();
		clk4_Divider_Mapped = div_to_factor[clk4_Divider];

		// Did we change the Divider setting? If so, reset the phase as per the master clock and recompute the factor
		if (clk4_Divider != clk4_Divider_Old)
		{
			clk4_Divider_Old = clk4_Divider;
			msr_Phase = 0.f;
			clk1_Phase = 0.f;
			clk2_Phase = 0.f;
			clk3_Phase = 0.f;
			clk4_Phase = 0.f;
		}
		// Phase data, 0..10V mapped to 0..1
		if (getInput(CLK_PHASE_INPUTS + 3).isConnected())
			clk4_Phase_Shift = getInput(CLK_PHASE_INPUTS + 3).getVoltage() * 0.1f;
		else
			clk4_Phase_Shift = getParam(CLK_PHASE_PARAMS + 3).getValue();

		// Gate Length Data = 10V mapped to range 5-95%
		if (getInput(CLK_GATE_INPUTS + 3).isConnected())
			clk4_Gate_Len = MIN_GATE_LEN + (MAX_GATE_LEN - MIN_GATE_LEN) * getInput(CLK_GATE_INPUTS + 3).getVoltage() * 0.1f;
		else
			clk4_Gate_Len = (int)getParam(CLK_GATE_PARAMS + 3).getValue();

		// Swing Amount Data = 10V mapped to a 5% range
		if (getInput(CLK_SWING_INPUTS + 3).isConnected())
			clk4_Swing_Amount = getInput(CLK_SWING_INPUTS + 3).getVoltage() * 0.5f;
		else
			clk4_Swing_Amount = (int)getParam(CLK_SWING_PARAMS + 3).getValue();

		// PROCESSING OF ALL INPUT STARTS HERE
		//
		// Compute Master Clock Frequency and derive the individual clocks
		msr_Freq = msr_BPM * One_Hz;
		clk1_Freq = msr_Freq * clk1_Divider_Mapped;
		clk2_Freq = msr_Freq * clk2_Divider_Mapped;
		clk3_Freq = msr_Freq * clk3_Divider_Mapped;
		clk4_Freq = msr_Freq * clk4_Divider_Mapped;

		// Was Run pressed or a pulse received on Run In?
		runButtonTriggered = runButtonTrigger.process(getParam(MSR_RUN_BTN_PARAM).getValue());
		runTriggered = runTrigger.process(getInput(MSR_RUN_INPUT).getVoltage(), 0.1f, 2.f);

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
		resetTriggered = resetTrigger.process(getInput(MSR_RESET_INPUT).getVoltage(), 0.1f, 2.f);

		if (resetButtonTriggered || resetTriggered)
		{
			// Start the reset trigger
			resetPulse.trigger(1e-3f);

			// Reset some vars
			is_Running = false;
			msr_Phase = 0.f;
			clk1_Phase = 0.f;
			clk2_Phase = 0.f;
			clk3_Phase = 0.f;
			clk4_Phase = 0.f;
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
				msr_Phase -= 1.f;
			clk1_Phase += clk1_Freq * args.sampleTime;
			if (clk1_Phase >= 1.f)
				clk1_Phase -= 1.f;
			clk2_Phase += clk2_Freq * args.sampleTime;
			if (clk2_Phase >= 1.f)
				clk2_Phase -= 1.f;
			clk3_Phase += clk3_Freq * args.sampleTime;
			if (clk3_Phase >= 1.f)
				clk3_Phase -= 1.f;
			clk4_Phase += clk4_Freq * args.sampleTime;
			if (clk4_Phase >= 1.f)
				clk4_Phase -= 1.f;

			// Master Clock
			// Compute the duration of the master gate
			msr_Gate_Duration = msr_Gate_Len / (msr_Freq * 100.f);
			// compute the Master Clock signal, which has no swing/phase shift
			msr_Gate_Voltage = master_Pulse(msr_Phase, msr_Gate_Len);
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

			// Clock 1
			// Compute the duration of the CLK1 gate
			clk1_Gate_Duration = clk1_Gate_Len / (clk1_Freq * 100.f);
			// Compute the derived clocks as per the pulse width, phase and random swing amount
			clk1_Gate_Voltage = clock_Pulse(clk1_Phase, clk1_Phase_Shift, clk1_Swing_value, clk1_Gate_Len);
			// Are we outputting a new CLK1 gate and not waiting for the previous one?
			if (clk1_Gate_Voltage > 0.0f && !clk1_Gate_Started)
			{
				clk1_Gate_Started = true;
				clk1_TrgPulse.trigger(1e-3f);
				clk1_GatePulse.trigger(clk1_Gate_Duration);
			}

			// Check the CLK1 trigger & gate pulse
			clk1_TrgState = clk1_TrgPulse.process(args.sampleTime);
			clk1_GateState = clk1_GatePulse.process(args.sampleTime);

			// If gate is running (gate pulse itself should be over) & wait finished and swing amount > 0, recompute swing
			if (clk1_Gate_Started && clk1_Phase > SWING_PHASE_TO_WAIT && !clk1_GateState)
			{
				// Recompute a new swing value as part of a cycle
				clk1_Swing_value = (1.f - 2.f * rack::random::uniform()) * clk1_Swing_Amount * 0.01;
				clk1_Gate_Started = false;
			}

			// Output the CLK1 gate & trigger & lights
			getOutput(CLK_GATE_OUTPUTS + 0).setVoltage((clk1_GateState) ? 10.f : 0.f);
			getOutput(CLK_TRIGGER_OUTPUTS + 0).setVoltage((clk1_TrgState) ? 10.f : 0.f);
			getLight(CLK_PULSE_LIGHTS + 0).setBrightnessSmooth(clk1_Gate_Voltage > 0.f, args.sampleTime);

			// Clock 2
			// Compute the duration of the CLK2 gate
			clk2_Gate_Duration = clk2_Gate_Len / (clk2_Freq * 100.f);
			// Compute the derived clocks as per the pulse width, phase and random swing amount
			clk2_Gate_Voltage = clock_Pulse(clk2_Phase, clk2_Phase_Shift, clk2_Swing_value, clk2_Gate_Len);
			// Are we outputting a new CLK2 gate and not waiting for the previous one?
			if (clk2_Gate_Voltage > 0.0f && !clk2_Gate_Started)
			{
				clk2_Gate_Started = true;
				clk2_TrgPulse.trigger(1e-3f);
				clk2_GatePulse.trigger(clk2_Gate_Duration);
			}

			// Check the CLK2 trigger & gate pulse
			clk2_TrgState = clk2_TrgPulse.process(args.sampleTime);
			clk2_GateState = clk2_GatePulse.process(args.sampleTime);

			// If gate is running (gate pulse itself should be over) & wait finished, recompute swing
			if (clk2_Gate_Started && clk2_Phase > SWING_PHASE_TO_WAIT && !clk2_GateState)
			{
				// Recompute a new swing value as part of a cycle
				clk2_Swing_value = (1.f - 2.f * rack::random::uniform()) * clk2_Swing_Amount * 0.01;
				clk2_Gate_Started = false;
			}

			// Output the CLK2 gate & trigger & lights
			getOutput(CLK_GATE_OUTPUTS + 1).setVoltage((clk2_GateState) ? 10.f : 0.f);
			getOutput(CLK_TRIGGER_OUTPUTS + 1).setVoltage((clk2_TrgState) ? 10.f : 0.f);
			getLight(CLK_PULSE_LIGHTS + 1).setBrightnessSmooth(clk2_Gate_Voltage > 0.f, args.sampleTime);

			// Clock 3
			// Compute the duration of the CLK3 gate
			clk3_Gate_Duration = clk3_Gate_Len / (clk3_Freq * 100.f);
			// Compute the derived clocks as per the pulse width, phase and random swing amount
			clk3_Gate_Voltage = clock_Pulse(clk3_Phase, clk3_Phase_Shift, clk3_Swing_value, clk3_Gate_Len);
			// Are we outputting a new CLK3 gate and not waiting for the previous one?
			if (clk3_Gate_Voltage > 0.0f && !clk3_Gate_Started)
			{
				clk3_Gate_Started = true;
				clk3_TrgPulse.trigger(1e-3f);
				clk3_GatePulse.trigger(clk3_Gate_Duration);
			}

			// Check the CLK3 trigger & gate pulse
			clk3_TrgState = clk3_TrgPulse.process(args.sampleTime);
			clk3_GateState = clk3_GatePulse.process(args.sampleTime);

			// If gate is running (gate pulse itself should be over) & wait finished, recompute swing
			if (clk3_Gate_Started && clk3_Phase > SWING_PHASE_TO_WAIT && !clk3_GateState)
			{
				// Recompute a new swing value as part of a cycle
				clk3_Swing_value = (1.f - 2.f * rack::random::uniform()) * clk3_Swing_Amount * 0.01;
				clk3_Gate_Started = false;
			}

			// Output the CLK3 gate & trigger & lights
			getOutput(CLK_GATE_OUTPUTS + 2).setVoltage((clk3_GateState) ? 10.f : 0.f);
			getOutput(CLK_TRIGGER_OUTPUTS + 2).setVoltage((clk3_TrgState) ? 10.f : 0.f);
			getLight(CLK_PULSE_LIGHTS + 2).setBrightnessSmooth(clk3_Gate_Voltage > 0.f, args.sampleTime);

			// Clock 4
			// Compute the duration of the CLK4 gate
			clk4_Gate_Duration = clk4_Gate_Len / (clk4_Freq * 100.f);
			// Compute the derived clocks as per the pulse width, phase and random swing amount
			clk4_Gate_Voltage = clock_Pulse(clk4_Phase, clk4_Phase_Shift, clk4_Swing_value, clk4_Gate_Len);
			// Are we outputting a new CLK4 gate and not waiting for the previous one?
			if (clk4_Gate_Voltage > 0.0f && !clk4_Gate_Started)
			{
				clk4_Gate_Started = true;
				clk4_TrgPulse.trigger(1e-3f);
				clk4_GatePulse.trigger(clk4_Gate_Duration);
			}

			// Check the CLK4 trigger & gate pulse
			clk4_TrgState = clk4_TrgPulse.process(args.sampleTime);
			clk4_GateState = clk4_GatePulse.process(args.sampleTime);

			// If gate is running (gate pulse itself should be over) & wait finished, recompute swing
			if (clk4_Gate_Started && clk4_Phase > SWING_PHASE_TO_WAIT && !clk4_GateState)
			{
				// Recompute a new swing value as part of a cycle
				clk4_Swing_value = (1.f - 2.f * rack::random::uniform()) * clk4_Swing_Amount * 0.01;
				clk4_Gate_Started = false;
			}

			// Output the CLK4 gate & trigger & lights
			getOutput(CLK_GATE_OUTPUTS + 3).setVoltage((clk4_GateState) ? 10.f : 0.f);
			getOutput(CLK_TRIGGER_OUTPUTS + 3).setVoltage((clk4_TrgState) ? 10.f : 0.f);
			getLight(CLK_PULSE_LIGHTS + 3).setBrightnessSmooth(clk4_Gate_Voltage > 0.f, args.sampleTime);
		}
		else // Not running
		{
			// Set phase to 0.0 to avoid the first pulse to be too short/quick
			msr_Phase = 0.f;
			msr_Gate_Started = false;
			clk1_Phase = 0.f;
			clk2_Phase = 0.f;
			clk3_Phase = 0.f;
			clk4_Phase = 0.f;
			clk1_Gate_Started = false;
			clk2_Gate_Started = false;
			clk3_Gate_Started = false;
			clk4_Gate_Started = false;

			// Reset all outputs & lights
			getOutput(MSR_GATE_OUTPUT).setVoltage(0.f);
			getOutput(CLK_GATE_OUTPUTS + 0).setVoltage(0.f);
			getOutput(CLK_GATE_OUTPUTS + 1).setVoltage(0.f);
			getOutput(CLK_GATE_OUTPUTS + 2).setVoltage(0.f);
			getOutput(CLK_GATE_OUTPUTS + 3).setVoltage(0.f);

			getLight(MSR_RUN_LIGHT).setBrightness(0.f);
			getLight(MSR_PULSE_LIGHT).setBrightness(0.f);
			getLight(CLK_PULSE_LIGHTS + 0).setBrightness(0.f);
			getLight(CLK_PULSE_LIGHTS + 1).setBrightness(0.f);
			getLight(CLK_PULSE_LIGHTS + 2).setBrightness(0.f);
			getLight(CLK_PULSE_LIGHTS + 3).setBrightness(0.f);
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
			clk_Divider = module->clk1_Divider;
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
			clk_Divider = module->clk2_Divider;
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
			clk_Divider = module->clk3_Divider;
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
			clk_Divider = module->clk4_Divider;
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
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(29.0, 26.75)), module, Ticker::MSR_BPM_INPUT));

		// Reset
		addParam(createLightParamCentered<VCVLightButton<MediumSimpleLight<STSBlueLight>>>(mm2px(Vec(44.0, 16.25)), module, Ticker::MSR_RESET_BTN_PARAM, Ticker::MSR_RESET_LIGHT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(44.0, 26.75)), module, Ticker::MSR_RESET_INPUT));

		// Run
		addParam(createLightParamCentered<VCVLightButton<MediumSimpleLight<STSBlueLight>>>(mm2px(Vec(59.0, 16.25)), module, Ticker::MSR_RUN_BTN_PARAM, Ticker::MSR_RUN_LIGHT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(59.0, 26.75)), module, Ticker::MSR_RUN_INPUT));

		// Gate Length
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(74.0, 16.25)), module, Ticker::MSR_GATE_PARAM));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(74.0, 26.75)), module, Ticker::MSR_GATE_INPUT));

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
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(29.5, 46.5)), module, Ticker::CLK_DIV_PARAMS));
		addChild(createLightCentered<SmallSimpleLight<STSYellowLight>>(mm2px(Vec(29.5, 46.5)), module, Ticker::CLK_PULSE_LIGHTS));

		// Phase shift
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(40.0, 46.5)), module, Ticker::CLK_PHASE_PARAMS));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(49.5, 46.5)), module, Ticker::CLK_PHASE_INPUTS));

		// Gate Length
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(62.5, 46.5)), module, Ticker::CLK_GATE_PARAMS));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(72.0, 46.5)), module, Ticker::CLK_GATE_INPUTS));

		// Swing Amount
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(85.0, 46.5)), module, Ticker::CLK_SWING_PARAMS));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(94.5, 46.5)), module, Ticker::CLK_SWING_INPUTS));

		// Outputs
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(108.0, 46.5)), module, Ticker::CLK_GATE_OUTPUTS));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(119.0, 46.5)), module, Ticker::CLK_TRIGGER_OUTPUTS));

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