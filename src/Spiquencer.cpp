#include "plugin.hpp"
#include "sts-base.hpp"
#include <math.h>

struct Spiquencer : Module
{
	enum ParamId
	{
		V_11_OUT_PARAM,
		V_21_OUT_PARAM,
		V_22_OUT_PARAM,
		PARAMS_LEN
	};
	enum InputId
	{
		POLY_IN_INPUT,
		INPUTS_LEN
	};
	enum OutputId
	{
		CH_11_OUT_OUTPUT,
		CH_21_OUT_OUTPUT,
		CH_22_OUT_OUTPUT,
		CH_31_OUT_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId
	{
		LGT_11_OUT_LIGHT,
		LGT_21_OUT_LIGHT,
		LGT_22_OUT_LIGHT,
		LGT_31_OUT_LIGHT,
		LIGHTS_LEN
	};

	Spiquencer()
	{
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(V_11_OUT_PARAM, 0.f, 1.f, 0.f, "");
		configParam(V_21_OUT_PARAM, 0.f, 1.f, 0.f, "");
		configParam(V_22_OUT_PARAM, 0.f, 1.f, 0.f, "");
		configInput(POLY_IN_INPUT, "");
		configOutput(CH_11_OUT_OUTPUT, "");
		configOutput(CH_21_OUT_OUTPUT, "");
		configOutput(CH_22_OUT_OUTPUT, "");
		configOutput(CH_31_OUT_OUTPUT, "");
	}

	void process(const ProcessArgs &args) override
	{
		lights[LGT_11_OUT_LIGHT].setBrightness(1.0);
		lights[LGT_21_OUT_LIGHT].setBrightness(1.0);
		lights[LGT_22_OUT_LIGHT].setBrightness(1.0);
		lights[LGT_31_OUT_LIGHT].setBrightness(1.0);

		return;
	}
};

struct SpiquencerWidget : ModuleWidget
{
	SpiquencerWidget(Spiquencer *module)
	{
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Spiquencer.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(61.687, 21.25)), module, Spiquencer::V_11_OUT_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(51.099, 39.0)), module, Spiquencer::V_21_OUT_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(70.68, 39.0)), module, Spiquencer::V_22_OUT_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(12.7, 21.25)), module, Spiquencer::POLY_IN_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(51.099, 21.25)), module, Spiquencer::CH_11_OUT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(40.383, 39.0)), module, Spiquencer::CH_21_OUT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(61.145, 39.0)), module, Spiquencer::CH_22_OUT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(29.555, 56.0)), module, Spiquencer::CH_31_OUT_OUTPUT));

		addChild(createLightCentered<SmallLight<STSRedLight>>(mm2px(Vec(61.681, 21.25)), module, Spiquencer::LGT_11_OUT_LIGHT));
		addChild(createLightCentered<SmallLight<STSRedLight>>(mm2px(Vec(51.099, 39.0)), module, Spiquencer::LGT_21_OUT_LIGHT));
		addChild(createLightCentered<SmallLight<STSRedLight>>(mm2px(Vec(70.814, 39.0)), module, Spiquencer::LGT_22_OUT_LIGHT));
		addChild(createLightCentered<SmallLight<STSRedLight>>(mm2px(Vec(29.5, 47.5)), module, Spiquencer::LGT_31_OUT_LIGHT));
	}
};

Model *modelSpiquencer = createModel<Spiquencer, SpiquencerWidget>("Spiquencer");