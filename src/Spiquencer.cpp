
#include "plugin.hpp"
#include "sts-base.hpp"
#include "notes.hpp"
#include <math.h>

struct Spiquencer : Module
{
	enum ParamId
	{
		V_11_PARAM,
		V_21_PARAM,
		V_22_PARAM,
		V_31_PARAM,
		V_32_PARAM,
		V_33_PARAM,
		V_41_PARAM,
		V_42_PARAM,
		V_43_PARAM,
		V_44_PARAM,
		V_51_PARAM,
		V_52_PARAM,
		V_53_PARAM,
		V_54_PARAM,
		V_55_PARAM,
		V_61_PARAM,
		V_62_PARAM,
		V_63_PARAM,
		V_64_PARAM,
		V_65_PARAM,
		V_66_PARAM,
		V_71_PARAM,
		V_72_PARAM,
		V_73_PARAM,
		V_74_PARAM,
		V_75_PARAM,
		V_76_PARAM,
		V_77_PARAM,
		V_81_PARAM,
		V_82_PARAM,
		V_83_PARAM,
		V_84_PARAM,
		V_85_PARAM,
		V_86_PARAM,
		V_87_PARAM,
		V_88_PARAM,
		PROBABILITY,
		PARAMS_LEN
	};
	enum InputId
	{
		GATE_IN_INPUT,
		RESET_IN_INPUT,
		INPUTS_LEN
	};
	enum OutputId
	{
		V_OUT_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId
	{
		LGT_11_LIGHT,
		LGT_21_LIGHT,
		LGT_22_LIGHT,
		LGT_31_LIGHT,
		LGT_32_LIGHT,
		LGT_33_LIGHT,
		LGT_41_LIGHT,
		LGT_42_LIGHT,
		LGT_43_LIGHT,
		LGT_44_LIGHT,
		LGT_51_LIGHT,
		LGT_52_LIGHT,
		LGT_53_LIGHT,
		LGT_54_LIGHT,
		LGT_55_LIGHT,
		LGT_61_LIGHT,
		LGT_62_LIGHT,
		LGT_63_LIGHT,
		LGT_64_LIGHT,
		LGT_65_LIGHT,
		LGT_66_LIGHT,
		LGT_71_LIGHT,
		LGT_72_LIGHT,
		LGT_73_LIGHT,
		LGT_74_LIGHT,
		LGT_75_LIGHT,
		LGT_76_LIGHT,
		LGT_77_LIGHT,
		LGT_81_LIGHT,
		LGT_82_LIGHT,
		LGT_83_LIGHT,
		LGT_84_LIGHT,
		LGT_85_LIGHT,
		LGT_86_LIGHT,
		LGT_87_LIGHT,
		LGT_88_LIGHT,
		LIGHTS_LEN
	};

	// Some class-wide parameters

	// Matrix to map col & row to a param number. -1 means param is not part of grid
	int mapRowColtoParam[8][8] = {
		{0, -1, -1, -1, -1, -1, -1, -1},
		{1, 2, -1, -1, -1, -1, -1, -1},
		{3, 4, 5, -1, -1, -1, -1, -1},
		{6, 7, 8, 9, -1, -1, -1, -1},
		{10, 11, 12, 13, 14, -1, -1, -1},
		{15, 16, 17, 18, 19, 20, -1, -1},
		{21, 22, 23, 24, 25, 26, 27, -1},
		{28, 29, 30, 31, 32, 33, 34, 35}};

	int curParam = 0;	 // current index in param mapping
	int oldParam = 0;	 // old index in param mapping
	int curSpikeRow = 0; // current row of spike that is sent to output
	int curSpikeCol = 0; // current spike within row that is sent to output

	int rootNote = 0;	 // Root Note as per the menu
	int rootScale = 0;	 // Scale selected
	int oldRootNote = 0; // To detect a change in note & scale
	int oldRootScale = 0;

	// Triggers to detect an external gate or reset signal
	dsp::SchmittTrigger gateTrigger;
	dsp::SchmittTrigger resetTrigger;

	bool gateTriggered = false;
	bool resetTriggered = false;

	// This is an Initialize, not a received Reset

	void onReset() override
	{
		// Reset the triggers
		gateTrigger.reset();
		resetTrigger.reset();
		// Reset the lights
		lights[curParam].setBrightness(0.f);
		lights[oldParam].setBrightness(0.f);
		// Reset the grid
		curParam = 0;
		oldParam = 0;
		// Reset some params
		params[PROBABILITY].setValue(0.f);
	}

	Spiquencer()
	{
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(V_11_PARAM, -4.f, 6.f, 0.f, "V/Oct 1-1");
		configParam(V_21_PARAM, -4.f, 6.f, 0.f, "V/Oct 2-1");
		configParam(V_22_PARAM, -4.f, 6.f, 0.f, "V/Oct 2-2");
		configParam(V_31_PARAM, -4.f, 6.f, 0.f, "V/Oct 3-1");
		configParam(V_32_PARAM, -4.f, 6.f, 0.f, "V/Oct 3-2");
		configParam(V_33_PARAM, -4.f, 6.f, 0.f, "V/Oct 3-3");
		configParam(V_41_PARAM, -4.f, 6.f, 0.f, "V/Oct 4-1");
		configParam(V_42_PARAM, -4.f, 6.f, 0.f, "V/Oct 4-2");
		configParam(V_43_PARAM, -4.f, 6.f, 0.f, "V/Oct 4-3");
		configParam(V_44_PARAM, -4.f, 6.f, 0.f, "V/Oct 4-4");
		configParam(V_51_PARAM, -4.f, 6.f, 0.f, "V/Oct 5-1");
		configParam(V_52_PARAM, -4.f, 6.f, 0.f, "V/Oct 5-2");
		configParam(V_53_PARAM, -4.f, 6.f, 0.f, "V/Oct 5-3");
		configParam(V_54_PARAM, -4.f, 6.f, 0.f, "V/Oct 5-4");
		configParam(V_55_PARAM, -4.f, 6.f, 0.f, "V/Oct 5-5");
		configParam(V_61_PARAM, -4.f, 6.f, 0.f, "V/Oct 6-1");
		configParam(V_62_PARAM, -4.f, 6.f, 0.f, "V/Oct 6-2");
		configParam(V_63_PARAM, -4.f, 6.f, 0.f, "V/Oct 6-3");
		configParam(V_64_PARAM, -4.f, 6.f, 0.f, "V/Oct 6-4");
		configParam(V_65_PARAM, -4.f, 6.f, 0.f, "V/Oct 6-5");
		configParam(V_66_PARAM, -4.f, 6.f, 0.f, "V/Oct 6-6");
		configParam(V_71_PARAM, -4.f, 6.f, 0.f, "V/Oct 7-1");
		configParam(V_72_PARAM, -4.f, 6.f, 0.f, "V/Oct 7-2");
		configParam(V_73_PARAM, -4.f, 6.f, 0.f, "V/Oct 7-3");
		configParam(V_74_PARAM, -4.f, 6.f, 0.f, "V/Oct 7-4");
		configParam(V_75_PARAM, -4.f, 6.f, 0.f, "V/Oct 7-5");
		configParam(V_76_PARAM, -4.f, 6.f, 0.f, "V/Oct 7-6");
		configParam(V_77_PARAM, -4.f, 6.f, 0.f, "V/Oct 7-7");
		configParam(V_81_PARAM, -4.f, 6.f, 0.f, "V/Oct 8-1");
		configParam(V_82_PARAM, -4.f, 6.f, 0.f, "V/Oct 8-2");
		configParam(V_83_PARAM, -4.f, 6.f, 0.f, "V/Oct 8-3");
		configParam(V_84_PARAM, -4.f, 6.f, 0.f, "V/Oct 8-4");
		configParam(V_85_PARAM, -4.f, 6.f, 0.f, "V/Oct 8-5");
		configParam(V_86_PARAM, -4.f, 6.f, 0.f, "V/Oct 8-6");
		configParam(V_87_PARAM, -4.f, 6.f, 0.f, "V/Oct 8-7");
		configParam(V_88_PARAM, -4.f, 6.f, 0.f, "V/Oct 8-8");
		configParam(PROBABILITY, 0.f, 1.f, 1.f, "%");
		configInput(GATE_IN_INPUT, "");
		configInput(RESET_IN_INPUT, "");
		configOutput(V_OUT_OUTPUT, "");

		onReset();
	}

	void process(const ProcessArgs &args) override
	{
		float stepVoltage;

		// Did we change scale or root note?
		if ((rootScale != oldRootScale) || (rootNote != oldRootNote))
		{
			oldRootScale = rootScale;
			oldRootNote = rootNote;

			int row, col, step = 0, note = 0;

			for (row = 0; row < 8; row++)
			{
				for (col = 0; col < 8; col++)
				{
					step = mapRowColtoParam[row][col];
					if (step > -1)
					{
						// note = (int)(12.f * rack::random::uniform());
						params[step].setValue(CHROMATIC_SCALES[rootNote][note]);
						note = (note + 1) % 12;
					}
				}
			}
		}

		// Was a pulse received on Gate In?
		gateTriggered = gateTrigger.process(inputs[GATE_IN_INPUT].getVoltage(), 0.1f, 2.f);
		// And is our probability allowing the gate to be processed?
		if (rack::random::uniform() > params[PROBABILITY].getValue())
			gateTriggered = false;

		// Was a pulse received on Reset In?
		resetTriggered = resetTrigger.process(inputs[RESET_IN_INPUT].getVoltage(), 0.1f, 2.f);
		if (resetTriggered)
			onReset();

		// Yes, so proceed to the next step
		if (gateTriggered)
		{
			// switch off prev step light
			lights[oldParam].setBrightness(0.f);
			oldParam = curParam;

			// read current param and copy it to output
			stepVoltage = params[curParam].getValue();
			outputs[V_OUT_OUTPUT].setVoltage(stepVoltage);
			lights[curParam].setBrightnessSmooth(gateTriggered, 0.25f * args.sampleTime);

			// Prepare for the next step
			curSpikeRow += 1;
			if (curSpikeRow > 7)
			{
				curSpikeRow = 0;
				curSpikeCol = 0;
				curParam = 0;
			}
			else
			{
				// If RND > 0.5, move column one right, else remain the same
				if (rack::random::uniform() > 0.5f)
					curSpikeCol += 1;
				curParam = mapRowColtoParam[curSpikeRow][curSpikeCol];
			}
		}
		else
			lights[curParam].setBrightness(0.f);
	}

	json_t *dataToJson() override
	{
		json_t *rootJ = json_object();

		json_object_set_new(rootJ, "Root Note", json_integer(rootNote));
		json_object_set_new(rootJ, "Scale", json_integer(rootScale));

		return rootJ;
	}

	void dataFromJson(json_t *rootJ) override
	{
		json_t *brootNoteJ = json_object_get(rootJ, "Root Note");
		json_t *rootScaleJ = json_object_get(rootJ, "Scale");

		if (brootNoteJ)
			rootNote = json_integer_value(brootNoteJ);
		if (rootScaleJ)
			rootScale = json_integer_value(rootScaleJ);
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

		// Step Params
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(51.162, 17.58)), module, Spiquencer::V_11_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(46.536, 24.981)), module, Spiquencer::V_21_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(56.104, 24.981)), module, Spiquencer::V_22_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(41.619, 32.585)), module, Spiquencer::V_31_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(51.188, 32.585)), module, Spiquencer::V_32_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(60.756, 32.585)), module, Spiquencer::V_33_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(36.831, 40.019)), module, Spiquencer::V_41_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(46.399, 40.019)), module, Spiquencer::V_42_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(55.968, 40.019)), module, Spiquencer::V_43_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(65.536, 40.019)), module, Spiquencer::V_44_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(32.145, 47.501)), module, Spiquencer::V_51_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(41.713, 47.501)), module, Spiquencer::V_52_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(51.282, 47.501)), module, Spiquencer::V_53_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(60.85, 47.501)), module, Spiquencer::V_54_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(70.419, 47.501)), module, Spiquencer::V_55_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(27.19, 55.028)), module, Spiquencer::V_61_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(36.758, 55.028)), module, Spiquencer::V_62_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(46.327, 55.028)), module, Spiquencer::V_63_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(55.895, 55.028)), module, Spiquencer::V_64_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(65.464, 55.028)), module, Spiquencer::V_65_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(75.032, 55.028)), module, Spiquencer::V_66_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(22.557, 62.606)), module, Spiquencer::V_71_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(32.125, 62.606)), module, Spiquencer::V_72_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(41.694, 62.606)), module, Spiquencer::V_73_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(51.262, 62.606)), module, Spiquencer::V_74_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(60.831, 62.606)), module, Spiquencer::V_75_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(70.399, 62.606)), module, Spiquencer::V_76_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(79.968, 62.606)), module, Spiquencer::V_77_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(17.554, 70.139)), module, Spiquencer::V_81_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(27.122, 70.139)), module, Spiquencer::V_82_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(36.691, 70.139)), module, Spiquencer::V_83_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(46.259, 70.139)), module, Spiquencer::V_84_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(55.828, 70.139)), module, Spiquencer::V_85_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(65.397, 70.139)), module, Spiquencer::V_86_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(74.965, 70.139)), module, Spiquencer::V_87_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(84.534, 70.139)), module, Spiquencer::V_88_PARAM));

		// General Params
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(32.0, 105.0)), module, Spiquencer::PROBABILITY));

		// Inputs
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.0, 105.0)), module, Spiquencer::GATE_IN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(21.0, 105.0)), module, Spiquencer::RESET_IN_INPUT));

		// Outputs
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(63.0, 105.0)), module, Spiquencer::V_OUT_OUTPUT));

		// Lights
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(51.156, 17.58)), module, Spiquencer::LGT_11_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(46.529, 24.981)), module, Spiquencer::LGT_21_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(56.098, 24.981)), module, Spiquencer::LGT_22_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(41.613, 32.585)), module, Spiquencer::LGT_31_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(51.181, 32.585)), module, Spiquencer::LGT_32_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(60.75, 32.585)), module, Spiquencer::LGT_33_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(36.824, 40.019)), module, Spiquencer::LGT_41_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(46.392, 40.019)), module, Spiquencer::LGT_42_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(55.961, 40.019)), module, Spiquencer::LGT_43_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(65.53, 40.019)), module, Spiquencer::LGT_44_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(32.138, 47.501)), module, Spiquencer::LGT_51_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(41.707, 47.501)), module, Spiquencer::LGT_52_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(51.275, 47.501)), module, Spiquencer::LGT_53_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(60.844, 47.501)), module, Spiquencer::LGT_54_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(70.412, 47.501)), module, Spiquencer::LGT_55_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(27.183, 55.028)), module, Spiquencer::LGT_61_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(36.752, 55.028)), module, Spiquencer::LGT_62_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(46.32, 55.028)), module, Spiquencer::LGT_63_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(55.889, 55.028)), module, Spiquencer::LGT_64_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(65.457, 55.028)), module, Spiquencer::LGT_65_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(75.026, 55.028)), module, Spiquencer::LGT_66_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(22.55, 62.606)), module, Spiquencer::LGT_71_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(32.119, 62.606)), module, Spiquencer::LGT_72_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(41.687, 62.606)), module, Spiquencer::LGT_73_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(51.256, 62.606)), module, Spiquencer::LGT_74_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(60.824, 62.606)), module, Spiquencer::LGT_75_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(70.393, 62.606)), module, Spiquencer::LGT_76_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(79.961, 62.606)), module, Spiquencer::LGT_77_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(17.547, 70.139)), module, Spiquencer::LGT_81_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(27.116, 70.139)), module, Spiquencer::LGT_82_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(36.684, 70.139)), module, Spiquencer::LGT_83_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(46.253, 70.139)), module, Spiquencer::LGT_84_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(55.821, 70.139)), module, Spiquencer::LGT_85_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(65.39, 70.139)), module, Spiquencer::LGT_86_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(74.959, 70.139)), module, Spiquencer::LGT_87_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(84.527, 70.139)), module, Spiquencer::LGT_88_LIGHT));
	}

	void appendContextMenu(Menu *menu) override
	{
		Spiquencer *module = getModule<Spiquencer>();

		menu->addChild(new MenuSeparator);

		menu->addChild(createIndexPtrSubmenuItem("Root Note", {"C", "C#/Db", "D", "D#/Eb", "E", "F", "F#/Gb", "G", "G#/Ab", "A", "A#/Bb", "B"}, &module->rootNote));
		menu->addChild(createIndexPtrSubmenuItem("Scale", {"Major", "Minor", "Minor Pentatonic"}, &module->rootScale));
	}
};

Model *modelSpiquencer = createModel<Spiquencer, SpiquencerWidget>("Spiquencer");