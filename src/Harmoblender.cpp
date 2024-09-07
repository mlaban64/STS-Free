#include "plugin.hpp"
#include "sts-base.hpp"
#include <math.h>

struct Harmoblender : Module
{
	enum ParamId
	{
		ENUMS(HRM_LVL_PARAMS, 16),
		ENUMS(HRM_PHASE_PARAMS, 16),
		ENUMS(HRM_MULT_PARAMS, 16),
		PITCH_PARAM,
		LVL_OUT_PARAM,
		PARAMS_LEN
	};
	enum InputId
	{
		ENUMS(HRM_LVL_INPUTS, 16),
		ENUMS(HRM_PHASE_INPUTS, 16),
		PITCH_IN_PARAM,
		V_OCT_IN_INPUT,
		INPUTS_LEN
	};
	enum OutputId
	{
		OUTPUT_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId
	{
		LIGHTS_LEN
	};

	// Some class-wide constants
	const float FREQ_MOD_MULTIPLIER = 0.1f;
	const float PHASE_MOD_MULTIPLIER = 0.1f;
	const float VOLUME_MOD_MULTIPLIER = 0.1f;

#define STS_NUM_WAVE_SAMPLES 1000

	// An array of values to represent the sine wave, as values in the range [-1.0, 1.0]. This can arguebly be regarded as a wavetable
	float sine_wave_lookup_table[STS_NUM_WAVE_SAMPLES];

	// local class variable
	float hrm_Lvl[16];			  // To store the level for this harmonic
	float hrm_Lvl_In[16];		  // To store the level modulation for this harmonic
	float hrm_Phase_Shift[16];	  // To store the phase shift for this harmonic
	float hrm_Phase_Shift_In[16]; // To store the phase shift modulation for this harmonic
	float hrm_Multiplication[16]; // To sore the multiplication factor for this harmonic
	float lvl_Multiplier = 0.f;	  // Global Level param, used to reduce the output level

	float freq = 0.f, pitch = 0.f, phase_shift = 0.f;
	int num_channels, idx;

	// Array of 16 phases to accomodate for polyphony
	float phase[16] = {0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f};

	// Maps phase & phase shift to an index in the wave table
	float STS_My_Sine(float phase, float phase_shift)
	{
		int index;

		// Compute the index by mapping phase + phase_shift across the total number of samples in the wave table
		index = (int)((phase + phase_shift) * STS_NUM_WAVE_SAMPLES);
		index = index % STS_NUM_WAVE_SAMPLES;

		return (sine_wave_lookup_table[index]);
	}

	void InitSine_Waves()
	{
		int i;

		// Populate the sine wave table
		// Filled with a full sine cycle, multiplied by 5.0 to reflect the default +/- 5V audio output levels
		for (i = 0; i < STS_NUM_WAVE_SAMPLES; i++)
		{
			sine_wave_lookup_table[i] = 5.0f * std::sin(M_2PI * ((float)i / STS_NUM_WAVE_SAMPLES));
		}
	}

	Harmoblender()
	{
		int i = 0;		 // index for harmoics loops
		std::string fmt; // string to format text
		char name[64];	 // string to format text

		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

		// Harmoic Blenders
		for (i = 0; i < 16; i++)
		{
			// Harmonic Blender Params
			(void)sprintf(name, "Harmonic %d Level", i + 1);
			fmt = name;
			configParam(HRM_LVL_PARAMS + i, -1.f, 1.f, 0.f, fmt);

			(void)sprintf(name, "Harmonic %d Phase Shift", i + 1);
			fmt = name;
			configParam(HRM_PHASE_PARAMS + i, 0.f, 0.999f, 0.f, fmt);

			(void)sprintf(name, "Harmonic %d Multiplication", i + 1);
			fmt = name;
			configParam(HRM_MULT_PARAMS + i, 0.f, 20.f, 0.f, fmt);
			getParamQuantity(HRM_MULT_PARAMS + i)->snapEnabled = true;

			(void)sprintf(name, "Harmonic %d Level CV (0..10V)", i + 1);
			fmt = name;
			configInput(HRM_LVL_INPUTS + i, fmt);

			(void)sprintf(name, "Harmonic %d Phase Shift CV (0..10V)", i + 1);
			fmt = name;
			configInput(HRM_PHASE_INPUTS + i, fmt);
		}

		// General In & Out
		configParam(PITCH_PARAM, 10.f, 20000.f, dsp::FREQ_C4, "Fixed pitch", " Hz");
		configInput(V_OCT_IN_INPUT, "Pitch (V//Oct)");
		configInput(PITCH_IN_PARAM, "Pitch Modulation");
		configParam(LVL_OUT_PARAM, 0.f, 1.f, 0.5f, "Ouput Level");
		configOutput(OUTPUT_OUTPUT, "Audio");

		InitSine_Waves();
	}

	void process(const ProcessArgs &args) override
	{
		int i = 0;				 // used to loop through harmonics
		float temp_Out = 0.f;	 // temp output for looping through harmonics
		float pitch_param = 0.f; // Pitch parameter

		// Get all the relevant values from the module UI

		// Compute the overall level output
		lvl_Multiplier = getParam(LVL_OUT_PARAM).getValue();

		// Recompute array values
		for (i = 0; i < 16; i++)
		{
			// Compute the phase shift as per the controls. Assume it is always in [0..1)]
			if (getInput(HRM_PHASE_INPUTS + i).isConnected())
				hrm_Phase_Shift[i] = abs(0.1f * getInput(HRM_PHASE_INPUTS + i).getVoltage());
			else
				hrm_Phase_Shift[i] = getParam(HRM_PHASE_PARAMS + i).getValue();
			// Compute the level as per the controls. Assume it is always in [0..1)]
			if (getInput(HRM_LVL_INPUTS + i).isConnected())
				hrm_Lvl[i] = 0.1f * getInput(HRM_LVL_INPUTS + i).getVoltage();
			else
				hrm_Lvl[i] = getParam(HRM_LVL_PARAMS + i).getValue();
			// Get the multiplication factors
			hrm_Multiplication[i] = getParam(HRM_MULT_PARAMS + i).getValue();
		}

		// Is the V-In connected?
		num_channels = getInput(V_OCT_IN_INPUT).getChannels();
		// First, match the # of output channels to the number of input channels, to ensure all other channels are reset to 0 V
		getOutput(OUTPUT_OUTPUT).setChannels(num_channels);

		if (num_channels == 0) // no V/Oct input, so use standard pitch of C4 (0 V)
		// If not, set the frequency as per the pitch parameter, using phase[0]
		{
			// Compute the pitch as per the controls
			pitch_param = getParam(PITCH_PARAM).getValue();
			if (getInput(PITCH_IN_PARAM).isConnected())
				freq = pitch_param + pitch_param * getInput(PITCH_IN_PARAM).getVoltage() * FREQ_MOD_MULTIPLIER;
			else
				freq = pitch_param;
			pitch_param = getParam(PITCH_PARAM).getValue();

			// limit the pitch if modulation takes it too extreme
			if (freq < 10.f)
				freq = 10.f;
			else if (freq > 20000.f)
				freq = 20000.f;

			// Accumulate the phase, make sure it rotates between 0.0 and 1.0
			phase[0] += freq * args.sampleTime;
			if (phase[0] >= 1.f)
				phase[0] -= 1.f;

			// Compute the wave by adding all harmonics
			// output to the correct channel, multiplied by the output level
			temp_Out = 0.f;
			for (i = 0; i < 16; i++)
			{
				temp_Out += hrm_Lvl[i] * STS_My_Sine(hrm_Multiplication[i] * phase[0], hrm_Phase_Shift[i]);
			}
			getOutput(OUTPUT_OUTPUT).setVoltage(lvl_Multiplier * temp_Out);
		}
		else
		{
			// Else, compute it as per the V/Oct input for each poly channel
			// Loop through all input channels
			for (idx = 0; idx < num_channels; idx++)
			{
				// Compute the pitch as per the controls
				pitch_param = getParam(PITCH_PARAM).getValue();
				pitch = getInput(V_OCT_IN_INPUT).getVoltage(idx);
				freq = pitch_param * std::pow(2.f, pitch);

				if (getInput(PITCH_IN_PARAM).isConnected())
					freq = freq + pitch_param * getInput(PITCH_IN_PARAM).getVoltage() * FREQ_MOD_MULTIPLIER;

				// limit the pitch if modulation takes it too extreme
				if (freq < 10.f)
					freq = 10.f;
				else if (freq > 20000.f)
					freq = 20000.f;

				// Accumulate the phase, make sure it rotates between 0.0 and 1.0
				phase[idx] += freq * args.sampleTime;
				if (phase[idx] >= 1.f)
					phase[idx] -= 1.f;

				// Compute the wave by adding all harmonics
				// output to the correct channel, multiplied by the output level
				temp_Out = 0.f;
				for (i = 0; i < 16; i++)
				{
					temp_Out += hrm_Lvl[i] * STS_My_Sine(hrm_Multiplication[i] * phase[idx], hrm_Phase_Shift[i]);
				}
				getOutput(OUTPUT_OUTPUT).setVoltage(lvl_Multiplier * temp_Out, idx);
			}
		}
	}
};

struct HarmoblenderWidget : ModuleWidget
{
	HarmoblenderWidget(Harmoblender *module)
	{
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Harmoblender.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		// H1 Panel
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(8.0, 15.5)), module, Harmoblender::HRM_LVL_PARAMS + 0));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(18.0, 15.5)), module, Harmoblender::HRM_PHASE_PARAMS + 0));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(28.0, 15.5)), module, Harmoblender::HRM_MULT_PARAMS + 0));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.0, 26.0)), module, Harmoblender::HRM_LVL_INPUTS + 0));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(18.0, 26.0)), module, Harmoblender::HRM_PHASE_INPUTS + 0));
		// H2 Panel
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(41.556, 15.5)), module, Harmoblender::HRM_LVL_PARAMS + 1));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(51.556, 15.5)), module, Harmoblender::HRM_PHASE_PARAMS + 1));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(61.556, 15.5)), module, Harmoblender::HRM_MULT_PARAMS + 1));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(41.556, 26.0)), module, Harmoblender::HRM_LVL_INPUTS + 1));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(51.556, 26.0)), module, Harmoblender::HRM_PHASE_INPUTS + 1));
		// H3 Panel
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(75.112, 15.5)), module, Harmoblender::HRM_LVL_PARAMS + 2));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(85.112, 15.5)), module, Harmoblender::HRM_PHASE_PARAMS + 2));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(95.112, 15.5)), module, Harmoblender::HRM_MULT_PARAMS + 2));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(75.112, 26.0)), module, Harmoblender::HRM_LVL_INPUTS + 2));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(85.112, 26.0)), module, Harmoblender::HRM_PHASE_INPUTS + 2));
		// H4 Panel
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(108.668, 15.5)), module, Harmoblender::HRM_LVL_PARAMS + 3));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(118.668, 15.5)), module, Harmoblender::HRM_PHASE_PARAMS + 3));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(128.668, 15.5)), module, Harmoblender::HRM_MULT_PARAMS + 3));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(108.668, 26.0)), module, Harmoblender::HRM_LVL_INPUTS + 3));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(118.668, 26.0)), module, Harmoblender::HRM_PHASE_INPUTS + 3));
		// H5 Panel
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(8.0, 39.171)), module, Harmoblender::HRM_LVL_PARAMS + 4));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(18.0, 39.171)), module, Harmoblender::HRM_PHASE_PARAMS + 4));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(28.0, 39.171)), module, Harmoblender::HRM_MULT_PARAMS + 4));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.0, 49.671)), module, Harmoblender::HRM_LVL_INPUTS + 4));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(18.0, 49.671)), module, Harmoblender::HRM_PHASE_INPUTS + 4));
		// H6 Panel
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(41.556, 39.171)), module, Harmoblender::HRM_LVL_PARAMS + 5));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(51.556, 39.171)), module, Harmoblender::HRM_PHASE_PARAMS + 5));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(61.556, 39.171)), module, Harmoblender::HRM_MULT_PARAMS + 5));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(41.556, 49.671)), module, Harmoblender::HRM_LVL_INPUTS + 5));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(51.556, 49.671)), module, Harmoblender::HRM_PHASE_INPUTS + 5));
		// H7 Panel
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(75.112, 39.171)), module, Harmoblender::HRM_LVL_PARAMS + 6));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(85.112, 39.171)), module, Harmoblender::HRM_PHASE_PARAMS + 6));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(95.112, 39.171)), module, Harmoblender::HRM_MULT_PARAMS + 6));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(75.112, 49.671)), module, Harmoblender::HRM_LVL_INPUTS + 6));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(85.112, 49.671)), module, Harmoblender::HRM_PHASE_INPUTS + 6));
		// H8 Panel
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(108.668, 39.171)), module, Harmoblender::HRM_LVL_PARAMS + 7));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(118.668, 39.171)), module, Harmoblender::HRM_PHASE_PARAMS + 7));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(128.668, 39.171)), module, Harmoblender::HRM_MULT_PARAMS + 7));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(108.668, 49.671)), module, Harmoblender::HRM_LVL_INPUTS + 7));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(118.668, 49.671)), module, Harmoblender::HRM_PHASE_INPUTS + 7));
		// H9 Panel
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(8.0, 62.671)), module, Harmoblender::HRM_LVL_PARAMS + 8));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(18.0, 62.671)), module, Harmoblender::HRM_PHASE_PARAMS + 8));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(28.0, 62.671)), module, Harmoblender::HRM_MULT_PARAMS + 8));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.0, 73.171)), module, Harmoblender::HRM_LVL_INPUTS + 8));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(18.0, 73.171)), module, Harmoblender::HRM_PHASE_INPUTS + 8));
		// H10 Panel
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(41.556, 62.671)), module, Harmoblender::HRM_LVL_PARAMS + 9));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(51.556, 62.671)), module, Harmoblender::HRM_PHASE_PARAMS + 9));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(61.556, 62.671)), module, Harmoblender::HRM_MULT_PARAMS + 9));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(41.556, 73.171)), module, Harmoblender::HRM_LVL_INPUTS + 9));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(51.556, 73.171)), module, Harmoblender::HRM_PHASE_INPUTS + 9));
		// H11 Panel
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(75.112, 62.671)), module, Harmoblender::HRM_LVL_PARAMS + 10));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(85.112, 62.671)), module, Harmoblender::HRM_PHASE_PARAMS + 10));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(95.112, 62.671)), module, Harmoblender::HRM_MULT_PARAMS + 10));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(75.112, 73.171)), module, Harmoblender::HRM_LVL_INPUTS + 10));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(85.112, 73.171)), module, Harmoblender::HRM_PHASE_INPUTS + 10));
		// H12 Panel
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(108.668, 62.671)), module, Harmoblender::HRM_LVL_PARAMS + 11));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(118.668, 62.671)), module, Harmoblender::HRM_PHASE_PARAMS + 11));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(128.668, 62.671)), module, Harmoblender::HRM_MULT_PARAMS + 11));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(108.668, 73.171)), module, Harmoblender::HRM_LVL_INPUTS + 11));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(118.668, 73.171)), module, Harmoblender::HRM_PHASE_INPUTS + 11));
		// H13 Panel
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(8.0, 86.341)), module, Harmoblender::HRM_LVL_PARAMS + 12));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(18.0, 86.341)), module, Harmoblender::HRM_PHASE_PARAMS + 12));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(28.0, 86.341)), module, Harmoblender::HRM_MULT_PARAMS + 12));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.0, 96.841)), module, Harmoblender::HRM_LVL_INPUTS + 12));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(18.0, 96.841)), module, Harmoblender::HRM_PHASE_INPUTS + 12));
		// H14 Panel
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(41.556, 86.341)), module, Harmoblender::HRM_LVL_PARAMS + 13));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(51.556, 86.341)), module, Harmoblender::HRM_PHASE_PARAMS + 13));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(61.556, 86.341)), module, Harmoblender::HRM_MULT_PARAMS + 13));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(41.556, 96.841)), module, Harmoblender::HRM_LVL_INPUTS + 13));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(51.556, 96.841)), module, Harmoblender::HRM_PHASE_INPUTS + 13));
		// H15 Panel
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(75.112, 86.341)), module, Harmoblender::HRM_LVL_PARAMS + 14));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(85.112, 86.341)), module, Harmoblender::HRM_PHASE_PARAMS + 14));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(95.112, 86.341)), module, Harmoblender::HRM_MULT_PARAMS + 14));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(75.112, 96.841)), module, Harmoblender::HRM_LVL_INPUTS + 14));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(85.112, 96.841)), module, Harmoblender::HRM_PHASE_INPUTS + 14));
		// H16 Panel
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(108.668, 86.341)), module, Harmoblender::HRM_LVL_PARAMS + 15));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(118.668, 86.341)), module, Harmoblender::HRM_PHASE_PARAMS + 15));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(128.668, 86.341)), module, Harmoblender::HRM_MULT_PARAMS + 15));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(108.668, 96.841)), module, Harmoblender::HRM_LVL_INPUTS + 15));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(118.668, 96.841)), module, Harmoblender::HRM_PHASE_INPUTS + 15));
		// General In & Put
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(89.0, 110.0)), module, Harmoblender::PITCH_PARAM));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(99.0, 110.0)), module, Harmoblender::PITCH_IN_PARAM));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(108.0, 110.0)), module, Harmoblender::V_OCT_IN_INPUT));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(128.0, 110.0)), module, Harmoblender::LVL_OUT_PARAM));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(119.5, 110.0)), module, Harmoblender::OUTPUT_OUTPUT));
	}
};

Model *modelHarmoblender = createModel<Harmoblender, HarmoblenderWidget>("Harmoblender");