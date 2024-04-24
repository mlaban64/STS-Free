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
		PARAMS_LEN
	};
	enum InputId
	{
		RESET_IN_INPUT,
		RUN_IN_INPUT,
		INPUTS_LEN
	};
	enum OutputId
	{
		GATE_MSR_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId
	{
		RESET_LIGHT,
		RUN_LIGHT,
		LIGHTS_LEN
	};

	// Some class-wide parameters

	dsp::BooleanTrigger runButtonTrigger;
	dsp::BooleanTrigger resetButtonTrigger;

	dsp::SchmittTrigger runTrigger;
	dsp::SchmittTrigger resetTrigger;

	bool is_Running = false; // is the clock running?

	const float One_Hz = 1.f / 60.f; // Factor to convert BPM to Hertz
	int master_BPM = 120;			 // current Master BPM value
	float master_Freq = 2.0;		 // current Master Frequence = BPM / 60
	float master_Phase = 0.0;		 // holds the phase of the Master Clock

	// A clock is basically a pulse, so using a modified part of the Pulse_VCO code here
	float STS_My_Pulse(float phase, float pulse_width)
	{
		if (phase < pulse_width)
			return (10.f);
		else
			return (0.f);
	}

	void onReset() override
	{
		master_BPM = 120;
		is_Running = false;
	}

	Ticker()
	{
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(BPM_PARAM, 10.f, 400.f, 120.f, "BPM");
		configInput(RESET_IN_INPUT, "Reset Trigger");
		configInput(RUN_IN_INPUT, "Run Trigger");
		configOutput(GATE_MSR_OUTPUT, "Clock Gate Out");

		onReset();
	}

	void process(const ProcessArgs &args) override
	{
		// Get all the values from the module UI
		master_BPM = (int)params[BPM_PARAM].getValue();

		// Compute Master Clock Frequence
		master_Freq = master_BPM * One_Hz;

		// Toggle run
		bool runButtonTriggered = runButtonTrigger.process(params[RUN_PARAM].getValue());
		bool runTriggered = runTrigger.process(inputs[RUN_IN_INPUT].getVoltage(), 0.1f, 2.f);

		if (runButtonTriggered || runTriggered)
		{
			is_Running ^= true;
		}

		if (is_Running)
		{
			// Accumulate the phase, make sure it rotates between 0.0 and 1.0
			master_Phase += master_Freq * args.sampleTime;
			if (master_Phase >= 1.f)
				master_Phase -= 1.f;

			// Output the pulse as per the pulse width and phase
			outputs[GATE_MSR_OUTPUT].setVoltage(STS_My_Pulse(master_Phase, 0.5f));
			lights[RUN_LIGHT].setBrightness(1.0f);
		}
		else // Not running
		{	 // Output the pulse as per the pulse width and phase
			outputs[GATE_MSR_OUTPUT].setVoltage(0.f);
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
		text = string::f("%d", display_BPM);
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

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(30.374, 16.836)), module, Ticker::BPM_PARAM));

		addParam(createLightParamCentered<VCVLightBezel<STSRedLight>>(mm2px(Vec(57.5, 17.5)), module, Ticker::RESET_PARAM, Ticker::RESET_LIGHT));
		addParam(createLightParamCentered<VCVLightButton<LargeSimpleLight<STSBlueLight>>>(mm2px(Vec(79.5, 17.5)), module, Ticker::RUN_PARAM, Ticker::RUN_LIGHT));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(67.0, 17.5)), module, Ticker::RESET_IN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(89.0, 17.5)), module, Ticker::RUN_IN_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(89.0, 43.5)), module, Ticker::GATE_MSR_OUTPUT));

		// mm2px(Vec(10.0, 10.0))
		// addChild(createWidget<Widget>(mm2px(Vec(7.032, 13.06))));

		Ticker_BPM_Display *display = createWidget<Ticker_BPM_Display>(mm2px(Vec(7.032, 13.06)));
		display->box.size = mm2px(Vec(15.0, 8.0));
		display->module = module;
		addChild(display);
	}
};

Model *modelTicker = createModel<Ticker, TickerWidget>("Ticker");