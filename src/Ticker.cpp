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
		INPUTS_LEN
	};
	enum OutputId
	{
		MSR_GATE_OUTPUT,
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

	dsp::BooleanTrigger runButtonTrigger;
	dsp::BooleanTrigger resetButtonTrigger;

	dsp::SchmittTrigger runTrigger;
	dsp::SchmittTrigger resetTrigger;

	bool is_Running = false; // is the clock running?
	bool is_Reset = false;	 // are we resetting?

	bool runButtonTriggered;
	bool runTriggered;
	bool resetButtonTriggered;
	bool resetTriggered;

	const float One_Hz = 1.f / 60.f; // Factor to convert BPM to Hertz

	int master_BPM = 120;		  // current Master BPM value
	float master_Freq = 2.f;	  // current Master Frequence = BPM / 60
	float master_Phase = 0.f;	  // holds the phase of the Master Clock
	float master_Gate_Len = 50.f; // Master Gate length in %
	float master_Gate_Voltage;	  // Output voltage for Master Gate

	float clk1_Divider = 1.f;	// current Master BPM value
	float clk1_Freq = 2.f;		// current Master Frequence = BPM / 60
	float clk1_Phase = 0.f;		// holds the phase of the Master Clock
	float clk1_Gate_Len = 50.f; // Master Gate length in %

	// A clock is basically a pulse, so using a modified part of the Pulse_VCO code here
	// phase_shift is used to delay/swing the pulse, pulse_width is a %%
	float STS_My_Pulse(float phase, float phase_shift, float pulse_width)
	{
		float local_phase;

		local_phase = (phase + phase_shift) * 100.f;
		if (local_phase < pulse_width)
			return (10.f);
		else
			return (0.f);
	}

	// This is an Initialize, not a RESET button push
	void onReset() override
	{
		// Set defaults
		master_BPM = 120;
		is_Running = false;
		master_Phase = 0.f;

		// Reset all outputs & lights
		outputs[MSR_GATE_OUTPUT].setVoltage(0.f);
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
		configOutput(MSR_GATE_OUTPUT, "Clock Gate Out");
		configOutput(MSR_RESET_OUTPUT, "Reset Out");
		configOutput(MSR_RUN_OUTPUT, "Run Out");

		// Clock 1 Params
		configParam(CLK1_DIV_PARAM, -64.f, 64.f, 1.f, "Divider", "", 0.f, 1.f, 0.f);
		paramQuantities[CLK1_DIV_PARAM]->snapEnabled = true;
		configParam(CLK1_PHASE_PARAM, 0.f, 1.f, 0.f, "Phase shift", " Cycle", 0.f, 1.f, 0.f);
		// Clock 1 Inputs
		configInput(CLK1_PHASE_IN_INPUT, "Phase shift Voltage (0..10V)");
		configInput(CLK1_GATE_LEN_IN_INPUT, "Gate Length Voltage (0..10V)");
		// Clock 1 Outputs
		configOutput(MSR_GATE_OUTPUT, "Clock Gate Out");

		onReset();
	}

	void process(const ProcessArgs &args) override
	{
		// Init some params
		is_Reset = false;

		// Get all the values from the module UI

		// Master Clock
		// BPM Data = 10V mapped to range 10-400 BPM
		if (inputs[MSR_BPM_IN_INPUT].isConnected())
		{
			master_BPM = MIN_BPM_PARAM + (MAX_BPM_PARAM - MIN_BPM_PARAM) * inputs[MSR_BPM_IN_INPUT].getVoltage() * 0.1f;
		}
		else
			master_BPM = (int)params[MSR_BPM_PARAM].getValue();

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
		// Phase data
		// Gate Length Data = 10V mapped to range 1-99%

		// The real clock processing starts here
		// Compute Master Clock Frequence
		master_Freq = master_BPM * One_Hz;

		// Was Run pressed or a pulse received on Run In?
		runButtonTriggered = runButtonTrigger.process(params[MSR_RUN_BTN_PARAM].getValue());
		runTriggered = runTrigger.process(inputs[MSR_RUN_IN_INPUT].getVoltage(), 0.1f, 2.f);

		if (runButtonTriggered || runTriggered)
		{
			is_Running ^= true;
		}

		// Was Reset pressed or a pulse received on Reset In?
		resetButtonTriggered = resetButtonTrigger.process(params[MSR_RESET_BTN_PARAM].getValue());
		resetTriggered = resetTrigger.process(inputs[MSR_RESET_IN_INPUT].getVoltage(), 0.1f, 2.f);

		if (resetButtonTriggered || resetTriggered)
		{
			is_Reset = true;
			is_Running = false;
			master_Phase = 0.f;
		}
		// Toggle the Reset light. The smaller the delta time, the slower the fade
		lights[MSR_RESET_LIGHT].setBrightnessSmooth(is_Reset, 0.25f * args.sampleTime);

		if (is_Running)
		{
			is_Reset = false;
			// Accumulate the phase, make sure it rotates between 0.0 and 1.0
			master_Phase += master_Freq * args.sampleTime;
			if (master_Phase >= 1.f)
				master_Phase -= 1.f;

			// Output the pulse as per the pulse width and phase
			master_Gate_Voltage = STS_My_Pulse(master_Phase, 0.0, master_Gate_Len);
			outputs[MSR_GATE_OUTPUT].setVoltage(master_Gate_Voltage);
			// Toggle the light. The smaller the delta time, the slower the fade
			lights[MSR_PULSE_LIGHT].setBrightnessSmooth(master_Gate_Voltage > 0.f, args.sampleTime);

			// Pass the Run = HIGH value (10V) to the RUN_MSR_OUTPUT
			outputs[MSR_RUN_OUTPUT].setVoltage(10.f);
			lights[MSR_RUN_LIGHT].setBrightness(1.f);
		}
		else // Not running
		{
			// Set phase to 0.0 to avoid the first pulse to be too short/quick
			master_Phase = 0.f;
			// Reset all outputs & lights
			outputs[MSR_GATE_OUTPUT].setVoltage(0.f);
			outputs[MSR_RUN_OUTPUT].setVoltage(0.f);
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
	void step() override
	{
		int display_DIV = 1;
		if (module)
			display_DIV = (int)module->clk1_Divider;
		if (display_DIV == 0)
			display_DIV = 1;
		if (display_DIV < 0)
			text = string::f("div %0*d", 2, -display_DIV);
		else
			text = string::f("mul %0*d", 2, display_DIV);
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
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(29.5, 17.5)), module, Ticker::MSR_BPM_PARAM));
		addChild(createLightCentered<SmallSimpleLight<STSYellowLight>>(mm2px(Vec(29.5, 17.5)), module, Ticker::MSR_PULSE_LIGHT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(39.0, 17.5)), module, Ticker::MSR_BPM_IN_INPUT));

		// Reset
		addParam(createLightParamCentered<VCVLightButton<MediumSimpleLight<STSBlueLight>>>(mm2px(Vec(54.5, 17.5)), module, Ticker::MSR_RESET_BTN_PARAM, Ticker::MSR_RESET_LIGHT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(64.0, 17.5)), module, Ticker::MSR_RESET_IN_INPUT));

		// Run
		addParam(createLightParamCentered<VCVLightButton<MediumSimpleLight<STSBlueLight>>>(mm2px(Vec(79.5, 17.5)), module, Ticker::MSR_RUN_BTN_PARAM, Ticker::MSR_RUN_LIGHT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(89.0, 17.5)), module, Ticker::MSR_RUN_IN_INPUT));

		// Gate Length
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(104.5, 17.5)), module, Ticker::MSR_GATE_LEN_PARAM));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(114.0, 17.5)), module, Ticker::MSR_GATE_LEN_IN_INPUT));

		// Clock Master Outputs
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(10.0, 40.781)), module, Ticker::MSR_GATE_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(40.0, 40.781)), module, Ticker::MSR_RESET_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(55.0, 40.781)), module, Ticker::MSR_RUN_OUTPUT));

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

		// Outputs
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(89.0, 63.5)), module, Ticker::CLK1_GATE_OUTPUT));
	}
};

Model *modelTicker = createModel<Ticker, TickerWidget>("Ticker");