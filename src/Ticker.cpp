#include "plugin.hpp"
#include "sts-base.hpp"
#include "Ticker.hpp"
#include <math.h>

struct Ticker : Module
{
	enum ParamId
	{
		BPM_PARAM,
		RESET_PARAM,
		RUN_PARAM,
		GATE_LEN_PARAM,
		CLK1_DIV_PARAM,
		PARAMS_LEN
	};
	enum InputId
	{
		BPM_IN_INPUT,
		RESET_IN_INPUT,
		RUN_IN_INPUT,
		GATE_LEN_IN_INPUT,
		INPUTS_LEN
	};
	enum OutputId
	{
		GATE_MSR_OUTPUT,
		RESET_MSR_OUTPUT,
		RUN_MSR_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId
	{
		RESET_LIGHT,
		RUN_LIGHT,
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

	const float One_Hz = 1.f / 60.f; // Factor to convert BPM to Hertz

	int master_BPM = 120;		  // current Master BPM value
	float master_Freq = 2.f;	  // current Master Frequence = BPM / 60
	float master_Phase = 0.f;	  // holds the phase of the Master Clock
	float master_Gate_Len = 50.f; // Master Gate length in %

	float CLK1_Divider = 2.f;	// current Master BPM value
	float CLK1_Freq = 2.f;		// current Master Frequence = BPM / 60
	float CLK1_Phase = 0.f;		// holds the phase of the Master Clock
	float CLK1_Gate_Len = 50.f; // Master Gate length in %

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

	void onReset() override
	{
		master_BPM = 120;
		is_Running = false;
		master_Phase = 0.f;
	}

	Ticker()
	{
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

		// Master Clock Params
		configParam(BPM_PARAM, 10.f, 400.f, 120.f, "BPM");
		configInput(BPM_IN_INPUT, "BPM Voltage (0..10V)");
		configInput(RESET_IN_INPUT, "Reset Trigger");
		configInput(RUN_IN_INPUT, "Run Trigger");
		configParam(GATE_LEN_PARAM, 1.f, 99.f, 50.f, "%");
		configInput(GATE_LEN_IN_INPUT, "Gate Length Voltage (0..10V)");
		configOutput(GATE_MSR_OUTPUT, "Clock Gate Out");

		// CLK1 Params
		configParam(CLK1_DIV_PARAM, -64.f, 64.f, 1.f, "Divider", "", 0.f, 1.f, 0.f);
		paramQuantities[CLK1_DIV_PARAM]->snapEnabled = true;

		onReset();
	}

	void process(const ProcessArgs &args) override
	{
		// Get all the values from the module UI

		// Master Clock BPM Data = 10V mapped to range 10-400 BPM
		if (inputs[BPM_IN_INPUT].isConnected())
		{
			master_BPM = MIN_BPM_PARAM + (MAX_BPM_PARAM - MIN_BPM_PARAM) * inputs[BPM_IN_INPUT].getVoltage() * 0.1f;
		}
		else
			master_BPM = (int)params[BPM_PARAM].getValue();

		// Master Clock Gate Length Data = 10V mapped to range 1-99%
		if (inputs[GATE_LEN_IN_INPUT].isConnected())
		{
			master_Gate_Len = 1.f + 98.f * inputs[GATE_LEN_IN_INPUT].getVoltage() * 0.1f;
		}
		else
			master_Gate_Len = (int)params[GATE_LEN_PARAM].getValue();

		// The real clock processing starts here
		// Compute Master Clock Frequence
		master_Freq = master_BPM * One_Hz;
		CLK1_Divider = params[CLK1_DIV_PARAM].getValue();

		// Was Run pressed or a pulse received on Run In?
		bool runButtonTriggered = runButtonTrigger.process(params[RUN_PARAM].getValue());
		bool runTriggered = runTrigger.process(inputs[RUN_IN_INPUT].getVoltage(), 0.1f, 2.f);

		if (runButtonTriggered || runTriggered)
		{
			is_Running ^= true;
		}

		// Was Reset pressed or a pulse received on Reset In?
		bool resetButtonTriggered = resetButtonTrigger.process(params[RESET_PARAM].getValue());
		bool resetTriggered = resetTrigger.process(inputs[RESET_IN_INPUT].getVoltage(), 0.1f, 2.f);

		if (resetButtonTriggered || resetTriggered)
		{
			is_Reset = true;
			is_Running = false;
			master_Phase = 0.f;
			lights[RESET_LIGHT].setBrightnessSmooth(1.f,0.1f,30.f);
		}

		if (is_Running)
		{
			is_Reset = false;
			// Accumulate the phase, make sure it rotates between 0.0 and 1.0
			master_Phase += master_Freq * args.sampleTime;
			if (master_Phase >= 1.f)
				master_Phase -= 1.f;

			// Output the pulse as per the pulse width and phase
			outputs[GATE_MSR_OUTPUT].setVoltage(STS_My_Pulse(master_Phase, 0.0, master_Gate_Len));
			// Pass the Run = HIGH value (10V) to the RUN_MSR_OUTPUT
			outputs[RUN_MSR_OUTPUT].setVoltage(10.f);
			lights[RUN_LIGHT].setBrightness(1.0f);
		}
		else // Not running
		{	 // Reset all outputs & lights
			outputs[GATE_MSR_OUTPUT].setVoltage(0.f);
			outputs[RUN_MSR_OUTPUT].setVoltage(0.f);
			lights[RUN_LIGHT].setBrightness(0.0f);
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
			display_DIV = (int)module->CLK1_Divider;
		if (display_DIV == 0)
			display_DIV = 1;
		if (display_DIV < 0)
			text = string::f("div %*u", 3, -display_DIV);
		else
			text = string::f("mul %*u", 3, display_DIV);
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
		Ticker_BPM_Display *Msr_BMP_display = createWidget<Ticker_BPM_Display>(mm2px(Vec(6.0, 13.5)));
		Msr_BMP_display->box.size = mm2px(Vec(18.0, 8.0));
		Msr_BMP_display->module = module;
		addChild(Msr_BMP_display);
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(30.0, 17.5)), module, Ticker::BPM_PARAM));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(39.0, 17.5)), module, Ticker::BPM_IN_INPUT));

		// Reset
		addParam(createLightParamCentered<VCVLightBezel<STSRedLight>>(mm2px(Vec(54.5, 17.5)), module, Ticker::RESET_PARAM, Ticker::RESET_LIGHT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(64.0, 17.5)), module, Ticker::RESET_IN_INPUT));

		// Run
		addParam(createLightParamCentered<VCVLightButton<LargeSimpleLight<STSBlueLight>>>(mm2px(Vec(79.5, 17.5)), module, Ticker::RUN_PARAM, Ticker::RUN_LIGHT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(89.0, 17.5)), module, Ticker::RUN_IN_INPUT));

		// Gate Length
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(104.5, 17.5)), module, Ticker::GATE_LEN_PARAM));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(114.0, 17.5)), module, Ticker::GATE_LEN_IN_INPUT));

		// Clock Master Outputs
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(10.0, 40.781)), module, Ticker::GATE_MSR_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(25.0, 40.781)), module, Ticker::RESET_MSR_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(40.0, 40.781)), module, Ticker::RUN_MSR_OUTPUT));

		// Clock 1 Panel
		Ticker_CLK1_Div_Display *CLK1_Div_display = createWidget<Ticker_CLK1_Div_Display>(mm2px(Vec(6.0, 60.0)));
		CLK1_Div_display->box.size = mm2px(Vec(18.0, 8.0));
		CLK1_Div_display->module = module;
		addChild(CLK1_Div_display);
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(29.0, 63.5)), module, Ticker::CLK1_DIV_PARAM));
	}
};

Model *modelTicker = createModel<Ticker, TickerWidget>("Ticker");