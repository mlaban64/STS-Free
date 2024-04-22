#include "plugin.hpp"
#include "sts-base.hpp"
#include "Ticker.hpp"
#include <math.h>

struct Ticker : Module
{
	enum ParamId
	{
		BPM_PARAM,
		PARAMS_LEN
	};
	enum InputId
	{
		RUN_IN_INPUT,
		INPUTS_LEN
	};
	enum OutputId
	{
		GATE_OUT_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId
	{
		LIGHTS_LEN
	};

	// Some class-wide parameters
	int cur_BPM; // current BPM value

	void onReset() override
	{
		cur_BPM = 120;
	}

	Ticker()
	{
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(BPM_PARAM, 10.f, 400.f, 120.f, "BPM");
		configInput(RUN_IN_INPUT, "Run Trigger");
		configOutput(GATE_OUT_OUTPUT, "Clock Gate Out");

		onReset();
	}

	void process(const ProcessArgs &args) override
	{
		// Get all the values from the module UI
		cur_BPM = (int)params[BPM_PARAM].getValue();

		return;
	}
};

struct Ticker_BPM_Display : BPM_Display
{
	Ticker *module;
	void step() override
	{
		int display_BPM = 99;
		if (module)
			display_BPM = module->cur_BPM;
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

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.332, 105.706)), module, Ticker::RUN_IN_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(45.421, 106.1)), module, Ticker::GATE_OUT_OUTPUT));

		// mm2px(Vec(10.0, 10.0))
		// addChild(createWidget<Widget>(mm2px(Vec(7.032, 13.06))));

		Ticker_BPM_Display *display = createWidget<Ticker_BPM_Display>(mm2px(Vec(7.032, 13.06)));
		display->box.size = mm2px(Vec(15.0, 8.0));
		display->module = module;
		addChild(display);
	}
};

Model *modelTicker = createModel<Ticker, TickerWidget>("Ticker");