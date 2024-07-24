
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
		V_91_PARAM,
		V_92_PARAM,
		V_93_PARAM,
		V_94_PARAM,
		V_95_PARAM,
		V_96_PARAM,
		V_97_PARAM,
		V_98_PARAM,
		V_99_PARAM,
		V_101_PARAM,
		V_102_PARAM,
		V_103_PARAM,
		V_104_PARAM,
		V_105_PARAM,
		V_106_PARAM,
		V_107_PARAM,
		V_108_PARAM,
		V_109_PARAM,
		V_1010_PARAM,
		PROBABILITY_PARAM,
		TRANSPOSE_PARAM,
		OCTAVES_PARAM,
		PARAMS_LEN
	};
	enum InputId
	{
		GATE_IN_INPUT,
		RESET_IN_INPUT,
		PROB_MOD_IN_INPUT,
		ROOT_IN_INPUT,
		SCALE_IN_INPUT,
		INPUTS_LEN
	};
	enum OutputId
	{
		V_OUT_OUTPUT,
		GATE_OUT_OUTPUT,
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
		LGT_91_LIGHT,
		LGT_92_LIGHT,
		LGT_93_LIGHT,
		LGT_94_LIGHT,
		LGT_95_LIGHT,
		LGT_96_LIGHT,
		LGT_97_LIGHT,
		LGT_98_LIGHT,
		LGT_99_LIGHT,
		LGT_101_LIGHT,
		LGT_102_LIGHT,
		LGT_103_LIGHT,
		LGT_104_LIGHT,
		LGT_105_LIGHT,
		LGT_106_LIGHT,
		LGT_107_LIGHT,
		LGT_108_LIGHT,
		LGT_109_LIGHT,
		LGT_1010_LIGHT,
		LIGHTS_LEN
	};

	// Some class-wide parameters

	// Matrix to map col & row to a param number. -1 means param is not part of grid
	int mapRowColtoParam[10][10] = {
		{0, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{1, 2, -1, -1, -1, -1, -1, -1, -1, -1},
		{3, 4, 5, -1, -1, -1, -1, -1, -1, -1},
		{6, 7, 8, 9, -1, -1, -1, -1, -1, -1},
		{10, 11, 12, 13, 14, -1, -1, -1, -1, -1},
		{15, 16, 17, 18, 19, 20, -1, -1, -1, -1},
		{21, 22, 23, 24, 25, 26, 27, -1, -1, -1},
		{28, 29, 30, 31, 32, 33, 34, 35, -1, -1},
		{36, 37, 38, 39, 40, 41, 42, 43, 44, -1},
		{45, 46, 47, 48, 49, 50, 51, 52, 53, 54}};

	int curParam = 0;	 // current index in param mapping
	int oldParam = 0;	 // old index in param mapping
	int curSpikeRow = 0; // current row of spike that is sent to output
	int curSpikeCol = 0; // current spike within row that is sent to output

	int rootNote = 0;		  // Root Note as per the menu
	int rootScale = 0;		  // Scale selected
	int scaleDirection = 0;	  // Steps to follow scale up/down/random/...
	float rootNote_CV = 0.f;  // To catch the root note CV input
	int rootNote_CV_Conv = 0; // rootNote_CV converted to integer, to map it to a menu item
	float transPose = 0.f;
	float ocTaves = 1.f;
	float oldTranspose = 0.f;
	float oldOctaves = 1.f;
	int oldRootNote = 0; // To detect a change in note & scale
	int oldRootScale = 0;
	int oldScaleDirection = 0;

	// Triggers to detect an external gate or reset signal
	dsp::SchmittTrigger gateTrigger;
	dsp::SchmittTrigger resetTrigger;

	bool gateTriggered = false;
	bool resetTriggered = false;
	bool changedParams = false;

	// This is an Initialize, not a received Reset

	void onReset() override
	{
		// Reset the triggers
		gateTrigger.reset();
		resetTrigger.reset();
		// Reset the lights
		getLight(curParam).setBrightness(0.f);
		getLight(oldParam).setBrightness(0.f);
		// Reset the grid
		curParam = 0;
		oldParam = 0;
		curSpikeCol = 0;
		curSpikeRow = 0;
		// Reset some params
		getParam(PROBABILITY_PARAM).setValue(1.f);
		getOutput(GATE_OUT_OUTPUT).setVoltage(0.f);
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
		configParam(V_91_PARAM, -4.f, 6.f, 0.f, "V/Oct 9-1");
		configParam(V_92_PARAM, -4.f, 6.f, 0.f, "V/Oct 9-2");
		configParam(V_93_PARAM, -4.f, 6.f, 0.f, "V/Oct 9-3");
		configParam(V_94_PARAM, -4.f, 6.f, 0.f, "V/Oct 9-4");
		configParam(V_95_PARAM, -4.f, 6.f, 0.f, "V/Oct 9-5");
		configParam(V_96_PARAM, -4.f, 6.f, 0.f, "V/Oct 9-6");
		configParam(V_97_PARAM, -4.f, 6.f, 0.f, "V/Oct 9-7");
		configParam(V_98_PARAM, -4.f, 6.f, 0.f, "V/Oct 9-8");
		configParam(V_99_PARAM, -4.f, 6.f, 0.f, "V/Oct 9-9");
		configParam(V_101_PARAM, -4.f, 6.f, 0.f, "V/Oct 10-1");
		configParam(V_102_PARAM, -4.f, 6.f, 0.f, "V/Oct 10-2");
		configParam(V_103_PARAM, -4.f, 6.f, 0.f, "V/Oct 10-3");
		configParam(V_104_PARAM, -4.f, 6.f, 0.f, "V/Oct 10-4");
		configParam(V_105_PARAM, -4.f, 6.f, 0.f, "V/Oct 10-5");
		configParam(V_106_PARAM, -4.f, 6.f, 0.f, "V/Oct 10-6");
		configParam(V_107_PARAM, -4.f, 6.f, 0.f, "V/Oct 10-7");
		configParam(V_108_PARAM, -4.f, 6.f, 0.f, "V/Oct 10-8");
		configParam(V_109_PARAM, -4.f, 6.f, 0.f, "V/Oct 10-9");
		configParam(V_1010_PARAM, -4.f, 6.f, 0.f, "V/Oct 10-10");
		configParam(PROBABILITY_PARAM, 0.f, 1.f, 1.f, "Probability");
		configParam(TRANSPOSE_PARAM, -3.f, 3.f, 0.f, "Transpose");
		getParamQuantity(TRANSPOSE_PARAM)->snapEnabled = true;
		configParam(OCTAVES_PARAM, 1.f, 6.f, 1.f, "# of Octaves");
		getParamQuantity(OCTAVES_PARAM)->snapEnabled = true;

		configInput(ROOT_IN_INPUT, "Root Note (CV In)");
		configInput(SCALE_IN_INPUT, "Scale (CV In)");
		configInput(GATE_IN_INPUT, "Gate In");
		configInput(RESET_IN_INPUT, "Reset In");
		configInput(PROB_MOD_IN_INPUT, "Probablility In");

		configOutput(V_OUT_OUTPUT, "V/Oct Out");
		configOutput(GATE_OUT_OUTPUT, "Gate Out");

		onReset();
	}

	void process(const ProcessArgs &args) override
	{
		float stepVoltage, prob_mod_Input;
		int row, col, step, oct, note, max_note, no_notes;

		// Get transposition & # of octaves
		transPose = getParam(TRANSPOSE_PARAM).getValue();
		ocTaves = getParam(OCTAVES_PARAM).getValue();

		// Get CV values for Root Note and Scale, if any...
		if (getInput(ROOT_IN_INPUT).isConnected())
		{
			// Assume 0..1V as CV input, so C4 till B4 are the voltages to select the root note.
			// This so you can select the root note via a keyboard
			rootNote_CV = getInput(ROOT_IN_INPUT).getVoltage();
			// Now map the CV to the selected menu number by taking the 100 x modulo 100, so we can map all 0..10V voltages to the C4-B4 range
			rootNote_CV_Conv = ((int)(100.f * rootNote_CV)) % 100;
			if (rootNote_CV_Conv == 0)
				rootNote = 0;
			else if (rootNote_CV_Conv == 8)
				rootNote = 1;
			else if (rootNote_CV_Conv == 16)
				rootNote = 2;
			else if (rootNote_CV_Conv == 25)
				rootNote = 3;
			else if (rootNote_CV_Conv == 33)
				rootNote = 4;
			else if (rootNote_CV_Conv == 41)
				rootNote = 5;
			else if (rootNote_CV_Conv == 50)
				rootNote = 6;
			else if (rootNote_CV_Conv == 58)
				rootNote = 7;
			else if (rootNote_CV_Conv == 66)
				rootNote = 8;
			else if (rootNote_CV_Conv == 75)
				rootNote = 9;
			else if (rootNote_CV_Conv == 83)
				rootNote = 10;
			else
				rootNote = 11;
		}

		if (getInput(SCALE_IN_INPUT).isConnected())
			// Assume 0..10V as CV input, scale it to fall into 0..(ALL SCALES - 1) range
			rootScale = 0.1f * (NO_ALL_SCALES_AND_ARPS - 0.1f) * getInput(SCALE_IN_INPUT).getVoltage();

		// Did we change scale, root note?, scale direction?
		if (rootScale != oldRootScale)
			changedParams = true;
		else if (rootNote != oldRootNote)
			changedParams = true;
		else if (scaleDirection != oldScaleDirection)
			changedParams = true;
		else if (transPose != oldTranspose)
			changedParams = true;
		else if (ocTaves != oldOctaves)
			changedParams = true;
		else if (scaleDirection != oldScaleDirection)
			changedParams = true;

		if (changedParams)
		{
			// Get the # of notes & max note number for the current scale
			no_notes = (int)ALL_SCALES_AND_ARPS[rootScale][rootNote][12];
			max_note = no_notes - 1;

			// Neutralize change detection
			oldRootScale = rootScale;
			oldRootNote = rootNote;
			oldScaleDirection = scaleDirection;
			oldTranspose = transPose;
			oldOctaves = ocTaves;
			changedParams = false;

			note = 0;
			oct = 0; // number of octaves (volts) to be added
			for (row = 0; row < 10; row++)
			{
				for (col = 0; col < 10; col++)
				{
					step = mapRowColtoParam[row][col];
					if (step > -1)
					{
						// Direction is up or down?
						if (scaleDirection == 0)
							getParam(step).setValue(ALL_SCALES_AND_ARPS[rootScale][rootNote][note] + transPose + oct);
						else
							getParam(step).setValue(ALL_SCALES_AND_ARPS[rootScale][rootNote][max_note - note] + transPose + oct);

						// Note modulo scale lenght, increase octave modulo # of octaves
						if (++note > max_note)
						{
							note = 0;
							if (++oct >= ocTaves)
								oct = 0;
						}
					}
				}
			}
		}

		// Was a pulse received on Gate In?
		gateTriggered = gateTrigger.process(getInput(GATE_IN_INPUT).getVoltage(), 0.1f, 2.f);
		// And is our probability allowing the gate to be processed? Connected PROB_MOD_IN takes precedence above PROBABILITY
		if (getInput(PROB_MOD_IN_INPUT).isConnected())
		{
			// Assume 0..10V on the PROB_MOD_IN connector, where 10V maps to probability 1, 0 or less to probability 0
			prob_mod_Input = getInput(PROB_MOD_IN_INPUT).getVoltage() * 0.1f;
			if (rack::random::uniform() > prob_mod_Input)
				gateTriggered = false;
			getParam(PROBABILITY_PARAM).setValue(prob_mod_Input);
		}
		else
		{
			if (rack::random::uniform() > getParam(PROBABILITY_PARAM).getValue())
				gateTriggered = false;
		}

		// Was a pulse received on Reset In?
		resetTriggered = resetTrigger.process(getInput(RESET_IN_INPUT).getVoltage(), 0.1f, 2.f);
		if (resetTriggered)
			onReset();

		// Yes, so proceed to the next step
		if (gateTriggered)
		{
			// Copy gate input to gate output
			if (gateTrigger.isHigh())
				getOutput(GATE_OUT_OUTPUT).setVoltage(10.f);
			// switch off prev step light
			getLight(oldParam).setBrightness(0.f);
			oldParam = curParam;

			// read current param and copy it to output
			stepVoltage = getParam(curParam).getValue();
			getOutput(V_OUT_OUTPUT).setVoltage(stepVoltage);
			getLight(curParam).setBrightnessSmooth(gateTriggered, 0.25f * args.sampleTime);

			// Prepare for the next step
			curSpikeRow += 1;
			if (curSpikeRow > 9)
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
		{
			// Copy gate input to gate output
			if (!gateTrigger.isHigh())
				getOutput(GATE_OUT_OUTPUT).setVoltage(0.f);
			getLight(curParam).setBrightness(0.f);
		}
	}

	json_t *dataToJson() override
	{
		json_t *rootJ = json_object();

		json_object_set_new(rootJ, "Root Note", json_integer(rootNote));
		json_object_set_new(rootJ, "Scale", json_integer(rootScale));
		json_object_set_new(rootJ, "Scale Direction", json_integer(scaleDirection));

		return rootJ;
	}

	void dataFromJson(json_t *rootJ) override
	{
		json_t *brootNoteJ = json_object_get(rootJ, "Root Note");
		json_t *rootScaleJ = json_object_get(rootJ, "Scale");
		json_t *scaleDirectionJ = json_object_get(rootJ, "Scale Direction");

		if (brootNoteJ)
			rootNote = json_integer_value(brootNoteJ);
		if (rootScaleJ)
			rootScale = json_integer_value(rootScaleJ);
		if (scaleDirectionJ)
			scaleDirection = json_integer_value(scaleDirectionJ);
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
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(51.162, 17.580)), module, Spiquencer::V_11_PARAM));
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
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(60.850, 47.501)), module, Spiquencer::V_54_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(70.419, 47.501)), module, Spiquencer::V_55_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(27.190, 55.028)), module, Spiquencer::V_61_PARAM));
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
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(12.500, 77.600)), module, Spiquencer::V_91_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(22.069, 77.600)), module, Spiquencer::V_92_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(31.637, 77.600)), module, Spiquencer::V_93_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(41.206, 77.600)), module, Spiquencer::V_94_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(50.774, 77.600)), module, Spiquencer::V_95_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(60.343, 77.600)), module, Spiquencer::V_96_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(69.911, 77.600)), module, Spiquencer::V_97_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(79.480, 77.600)), module, Spiquencer::V_98_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(89.100, 77.600)), module, Spiquencer::V_99_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(07.471, 85.100)), module, Spiquencer::V_101_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(17.040, 85.100)), module, Spiquencer::V_102_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(26.608, 85.100)), module, Spiquencer::V_103_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(36.177, 85.100)), module, Spiquencer::V_104_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(45.746, 85.100)), module, Spiquencer::V_105_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(55.314, 85.100)), module, Spiquencer::V_106_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(64.883, 85.100)), module, Spiquencer::V_107_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(74.451, 85.100)), module, Spiquencer::V_108_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(84.071, 85.100)), module, Spiquencer::V_109_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(94.000, 85.100)), module, Spiquencer::V_1010_PARAM));

		// General Params
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(32.0, 105.0)), module, Spiquencer::PROBABILITY_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(54.0, 105.0)), module, Spiquencer::TRANSPOSE_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(65.0, 105.0)), module, Spiquencer::OCTAVES_PARAM));

		// Root/SCale Inputs (CV)
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.0, 21.0)), module, Spiquencer::ROOT_IN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(21.0, 21.0)), module, Spiquencer::SCALE_IN_INPUT));

		// Inputs
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.0, 105.0)), module, Spiquencer::GATE_IN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(21.0, 105.0)), module, Spiquencer::RESET_IN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(43.0, 105.0)), module, Spiquencer::PROB_MOD_IN_INPUT));

		// Outputs
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(81.0, 105.0)), module, Spiquencer::V_OUT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(92.0, 105.0)), module, Spiquencer::GATE_OUT_OUTPUT));

		// Lights
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(51.156, 17.580)), module, Spiquencer::LGT_11_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(46.529, 24.981)), module, Spiquencer::LGT_21_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(56.098, 24.981)), module, Spiquencer::LGT_22_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(41.613, 32.585)), module, Spiquencer::LGT_31_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(51.181, 32.585)), module, Spiquencer::LGT_32_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(60.750, 32.585)), module, Spiquencer::LGT_33_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(36.824, 40.019)), module, Spiquencer::LGT_41_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(46.392, 40.019)), module, Spiquencer::LGT_42_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(55.961, 40.019)), module, Spiquencer::LGT_43_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(65.530, 40.019)), module, Spiquencer::LGT_44_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(32.138, 47.501)), module, Spiquencer::LGT_51_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(41.707, 47.501)), module, Spiquencer::LGT_52_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(51.275, 47.501)), module, Spiquencer::LGT_53_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(60.844, 47.501)), module, Spiquencer::LGT_54_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(70.412, 47.501)), module, Spiquencer::LGT_55_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(27.183, 55.028)), module, Spiquencer::LGT_61_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(36.752, 55.028)), module, Spiquencer::LGT_62_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(46.320, 55.028)), module, Spiquencer::LGT_63_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(55.889, 55.028)), module, Spiquencer::LGT_64_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(65.457, 55.028)), module, Spiquencer::LGT_65_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(75.026, 55.028)), module, Spiquencer::LGT_66_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(22.550, 62.606)), module, Spiquencer::LGT_71_LIGHT));
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
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(65.390, 70.139)), module, Spiquencer::LGT_86_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(74.959, 70.139)), module, Spiquencer::LGT_87_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(84.527, 70.139)), module, Spiquencer::LGT_88_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(12.493, 77.600)), module, Spiquencer::LGT_91_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(22.062, 77.600)), module, Spiquencer::LGT_92_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(31.631, 77.600)), module, Spiquencer::LGT_93_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(41.199, 77.600)), module, Spiquencer::LGT_94_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(50.768, 77.600)), module, Spiquencer::LGT_95_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(60.336, 77.600)), module, Spiquencer::LGT_96_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(69.905, 77.600)), module, Spiquencer::LGT_97_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(79.473, 77.600)), module, Spiquencer::LGT_98_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(89.093, 77.600)), module, Spiquencer::LGT_99_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(07.465, 85.100)), module, Spiquencer::LGT_101_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(17.033, 85.100)), module, Spiquencer::LGT_102_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(26.602, 85.100)), module, Spiquencer::LGT_103_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(36.170, 85.100)), module, Spiquencer::LGT_104_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(45.739, 85.100)), module, Spiquencer::LGT_105_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(55.208, 85.100)), module, Spiquencer::LGT_106_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(64.876, 85.100)), module, Spiquencer::LGT_107_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(74.445, 85.100)), module, Spiquencer::LGT_108_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(84.065, 85.100)), module, Spiquencer::LGT_109_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<STSRedLight>>(mm2px(Vec(93.993, 85.100)), module, Spiquencer::LGT_1010_LIGHT));
	}

	void appendContextMenu(Menu *menu) override
	{
		Spiquencer *module = getModule<Spiquencer>();

		menu->addChild(new MenuSeparator);

		menu->addChild(createIndexPtrSubmenuItem("Root Note", {"C", "C#/Db", "D", "D#/Eb", "E", "F", "F#/Gb", "G", "G#/Ab", "A", "A#/Bb", "B"}, &module->rootNote));
		menu->addChild(createIndexPtrSubmenuItem("Scale/Arp", {"Chromatic", "Minor Pentatonic", "Major Pentatonic", "Minor Blues", "Major Blues", "Ionian/Major", "Dorian", "Phrygian", "Lydian", "Mixolydian", "Aeolian/Minor", "Locrian", "Arp:Major", "Arp:Minor", "Arp:Dim", "Arp:Aug", "Arp:sus2", "Arp:sus4", "Arp:7", "Arp:maj7", "Arp: min7", "Arp:maj6", "Arp: min6", "Arp:add9", "Arp:min(add9)", "Arp:min7b5", "Arp:9", "Arp:min9"},
												 &module->rootScale));
		menu->addChild(createIndexPtrSubmenuItem("Scale Direction", {"Up", "Down"}, &module->scaleDirection));
	}
};

Model *modelSpiquencer = createModel<Spiquencer, SpiquencerWidget>("Spiquencer");