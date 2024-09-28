#include "plugin.hpp"
#include "sts-base.hpp"
#include <math.h>

struct SuperZzzaw : Module
{
	enum ParamId
	{
		ENUMS(SZZ_LVL_PARAMS, 8),
		ENUMS(SZZ_PHASE_PARAMS, 8),
		ENUMS(SZZ_DETUNE_PARAMS, 8),
		ENUMS(SZZ_PAN_PARAMS, 8),
		PITCH_PARAM,
		LVL_OUT_PARAM,
		PARAMS_LEN
	};
	enum InputId
	{
		ENUMS(SZZ_LVL_INPUTS, 8),
		ENUMS(SZZ_PHASE_INPUTS, 8),
		ENUMS(SZZ_DETUNE_INPUTS, 8),
		ENUMS(SZZ_PAN_INPUTS, 8),
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
	const float FREQ_MOD_MULTIPLIER = 0.5f;
	const float PHASE_MOD_MULTIPLIER = 0.1f;
	const float VOLUME_MOD_MULTIPLIER = 0.1f;

#define STS_NUM_WAVE_SAMPLES 1000
#define STS_DEF_NUM_HARMONICS 10

	// Some class-wide variables
	int rampDir = 0;
	int bandLimited = 0;
	int num_Harmonics = STS_DEF_NUM_HARMONICS;
	int last_menu_num_Harmonics = STS_DEF_NUM_HARMONICS - 1;
	int menu_num_Harmonics = STS_DEF_NUM_HARMONICS - 1;

	// An array of values to represent the sawtooth wave, as values in the range [-1.0, 1.0]. This can arguebly be regarded as a wavetable
	// 4 arrays are used: Band-limited (# of harmonics set as per STS_NUM_SAW_HARMONICS) and Band-unlimited, approachng the mathematicsl sawtooth
	float saw_bl_up_wave_lookup_table[STS_NUM_WAVE_SAMPLES];
	float saw_bl_down_wave_lookup_table[STS_NUM_WAVE_SAMPLES];
	float saw_bu_up_wave_lookup_table[STS_NUM_WAVE_SAMPLES];
	float saw_bu_down_wave_lookup_table[STS_NUM_WAVE_SAMPLES];

	// local class variable declarations
	float pitch_param, phase_param, volume_param;
	float freq = 0.f, pitch = 0.f, phase_shift = 0.f, volume_out = 0.f;
	float freq_mod = 0.f, phase_mod = 0.f, volume_mod = 0.f;
	float freq_mod_attn = 0.f, phase_mod_attn = 0.f, volume_mod_attn = 0.f;
	int num_channels, idx;

	// Array of 16 phases to accomodate for polyphony
	float phase[16] = {0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f};

	// Maps phase & phase shift to an index in the wave table
	float STS_My_Saw(float phase, float phase_shift)
	{
		int index;

		// Compute the index by mapping phase + phase_shift across the total number of samples in the wave table
		index = (int)((phase + phase_shift) * STS_NUM_WAVE_SAMPLES);
		index = index % STS_NUM_WAVE_SAMPLES;

		// ramp is up?
		if (rampDir == 0)
		{
			// Band-unlimited?
			if (bandLimited)
				return (saw_bl_up_wave_lookup_table[index]);
			else
				return (saw_bu_up_wave_lookup_table[index]);
		}
		else // ramp is down
		{
			// Band-unlimited?
			if (bandLimited)
				return (saw_bl_down_wave_lookup_table[index]);
			else
				return (saw_bu_down_wave_lookup_table[index]);
		}
	}

	// Custom OnReset() to initialize wave tables and set some default values
	void onReset() override
	{
		rampDir = 0;
		bandLimited = 0;
		num_Harmonics = STS_DEF_NUM_HARMONICS;
		menu_num_Harmonics = STS_DEF_NUM_HARMONICS - 1;
		last_menu_num_Harmonics = STS_DEF_NUM_HARMONICS - 1;
		InitSaw_Waves(num_Harmonics);
	}

	void InitSaw_Waves(int num_Harm)
	{
		int i, j;
		float iter, harmonic, h_factor, max_harmonic;

		// Populate the band-unlimited sawtooth wave tables
		// Filled with a full sawtooth cycle, multiplied by 5.0 to reflect the default +/- 5V audio output levels
		for (i = 0; i < STS_NUM_WAVE_SAMPLES; i++)
		{
			// Up ramp
			saw_bu_up_wave_lookup_table[i] = -5.0f + 10.0f * ((float)i / STS_NUM_WAVE_SAMPLES);
			// Down ramp
			saw_bu_down_wave_lookup_table[STS_NUM_WAVE_SAMPLES - i - 1] = saw_bu_up_wave_lookup_table[i];
		}

		// Using harmonic sine waves to mimic a band-limited sawtooth wave to limit aliases
		// Sawtooth = all harmonics 2nd 3rd 4th &c. where 2nd = ½, 3rd = ¹/₃ & 4th = ¼ volume.
		for (i = 0; i < STS_NUM_WAVE_SAMPLES; i++)
		{
			iter = M_2PI * ((float)i / STS_NUM_WAVE_SAMPLES);
			saw_bl_down_wave_lookup_table[i] = 0.0f;

			// Now loop through the harmonics until...
			for (j = 1; j <= num_Harm; j++)
			{
				h_factor = (float)j;
				harmonic = std::sin(h_factor * iter) / h_factor;
				saw_bl_down_wave_lookup_table[i] = saw_bl_down_wave_lookup_table[i] + harmonic;
			}
		}

		// Now fnd the max harmonic value to correct the output voltage between -5.0 and 5.0 V
		// Find tha largest harmoic first
		max_harmonic = 0.0f;
		for (i = 0; i < STS_NUM_WAVE_SAMPLES; i++)
		{
			if (saw_bl_down_wave_lookup_table[i] > max_harmonic)
				max_harmonic = saw_bl_down_wave_lookup_table[i];
		}
		// Then correct amplitude and make the inverse saw too
		for (i = 0; i < STS_NUM_WAVE_SAMPLES; i++)
		{
			saw_bl_down_wave_lookup_table[i] *= (5.0f / max_harmonic);
			saw_bl_up_wave_lookup_table[STS_NUM_WAVE_SAMPLES - i - 1] = saw_bl_down_wave_lookup_table[i];
		}
	}

	SuperZzzaw()
	{
		int i = 0;		 // index for harmonics loops
		std::string fmt; // string to format text
		char name[64];	 // string to format text

		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

		// Harmonic Blenders
		for (i = 0; i < 8; i++)
		{
			// Harmonic Blender Params
			(void)sprintf(name, "Sawtooth %d Level", i + 1);
			fmt = name;
			configParam(SZZ_LVL_PARAMS + i, -1.f, 1.f, 0.f, fmt);

			(void)sprintf(name, "Sawtooth %d Phase Shift", i + 1);
			fmt = name;
			configParam(SZZ_PHASE_PARAMS + i, 0.f, 0.999f, 0.f, fmt);

			(void)sprintf(name, "Sawtooth %d Detune (%%)", i + 1);
			fmt = name;
			configParam(SZZ_DETUNE_PARAMS + i, -10.f, 10.f, 0.f, fmt);

			(void)sprintf(name, "Sawtooth %d Pan", i + 1);
			fmt = name;
			configParam(SZZ_PAN_PARAMS + i, -1.f, 1.f, 0.f, fmt);

			(void)sprintf(name, "Sawtooth %d Level CV", i + 1);
			fmt = name;
			configInput(SZZ_LVL_INPUTS + i, fmt);

			(void)sprintf(name, "Sawtooth %d Phase CV", i + 1);
			fmt = name;
			configInput(SZZ_PHASE_INPUTS + i, fmt);

			(void)sprintf(name, "Sawtooth %d Detunr CV", i + 1);
			fmt = name;
			configInput(SZZ_DETUNE_INPUTS + i, fmt);

			(void)sprintf(name, "Sawtooth %d Pan CV", i + 1);
			fmt = name;
			configInput(SZZ_PAN_INPUTS + i, fmt);
		}

		// General In & Out
		configParam(PITCH_PARAM, 10.f, 20000.f, dsp::FREQ_C4, "Fixed pitch", " Hz");
		configInput(V_OCT_IN_INPUT, "Pitch (V//Oct)");
		configInput(PITCH_IN_PARAM, "Pitch Modulation");
		configParam(LVL_OUT_PARAM, 0.f, 1.f, 0.5f, "Ouput Level");
		configOutput(OUTPUT_OUTPUT, "Audio");

		InitSaw_Waves(STS_DEF_NUM_HARMONICS);
	}

	void process(const ProcessArgs &args) override
	{
		// Check if we need to recompute the wave tables
		if (last_menu_num_Harmonics != menu_num_Harmonics)
		{
			num_Harmonics = menu_num_Harmonics + 1;
			InitSaw_Waves(num_Harmonics);
			last_menu_num_Harmonics = menu_num_Harmonics;
		}

		// Get all the values from the module UI
		pitch_param = getParam(PITCH_PARAM).getValue();
		/*
		phase_param = getParam(PHASE_PARAM).getValue();
		volume_param = getParam(VOLUME_PARAM).getValue();
		freq_mod_attn = getParam(FM_ATTN_PARAM).getValue();
		phase_mod_attn = getParam(PM_ATTN_PARAM).getValue();
		volume_mod_attn = getParam(VM_ATTN_PARAM).getValue();
		freq_mod = getInput(FM_IN_INPUT).getVoltage();
		phase_mod = getInput(PM_IN_INPUT).getVoltage();
		volume_mod = getInput(VM_IN_INPUT).getVoltage();

		// Compute the volume output as per the controls
		if (getInput(VM_IN_INPUT).isConnected())
			volume_out = volume_param + volume_mod * volume_mod_attn * VOLUME_MOD_MULTIPLIER;
		else
			volume_out = volume_param;

		// Compute the phase shift as per the controls. Make sure it is not negative, as this may happen with modulation with a bipolar signal
		if (getInput(PM_IN_INPUT).isConnected())
		{
			phase_shift = phase_param + phase_mod * phase_mod_attn * PHASE_MOD_MULTIPLIER;
			if (phase_shift < 0.0f)
				phase_shift += 1.0f;
		}
		else
			phase_shift = phase_param;

		// Is the V-In connected?
		num_channels = getInput(V_OCT_IN_INPUT).getChannels();
		// First, match the # of output channels to the number of input channels, to ensure all other channels are reset to 0 V
		getOutput(OUTPUT_OUTPUT).setChannels(num_channels);

		if (num_channels == 0)
		// If not, set the frequency as per the pitch parameter, using phase[0]
		{
			// Compute the pitch as per the controls
			if (getInput(FM_IN_INPUT).isConnected())
				freq = pitch_param + pitch_param * freq_mod * freq_mod_attn * FREQ_MOD_MULTIPLIER;
			else
				freq = pitch_param;

			// limit the pitch if modulation takes it too extreme
			if (freq < 10.f)
				freq = 10.f;
			else if (freq > 20000.f)
				freq = 20000.f;

			// Accumulate the phase, make sure it rotates between 0.0 and 1.0
			phase[0] += freq * args.sampleTime;
			if (phase[0] >= 1.f)
				phase[0] -= 1.f;

			// Compute the wave via the wave table,
			// output to the correct channel, multiplied by the output volume
			getOutput(OUTPUT_OUTPUT).setVoltage(volume_out * STS_My_Saw(phase[0], phase_shift));
		}
		else
		{
			// Else, compute it as per the V/Oct input for each poly channel
			// Loop through all input channels
			for (idx = 0; idx < num_channels; idx++)
			{
				pitch = getInput(V_OCT_IN_INPUT).getVoltage(idx);
				freq = pitch_param * std::pow(2.f, pitch);

				// Compute the pitch as per the controls
				if (getInput(FM_IN_INPUT).isConnected())
					freq = freq + freq * freq_mod * freq_mod_attn * FREQ_MOD_MULTIPLIER;

				// limit the pitch if modulation takes it too extreme
				if (freq < 10.f)
					freq = 10.f;
				else if (freq > 20000.f)
					freq = 20000.f;

				// Accumulate the phase, make sure it rotates between 0.0 and 1.0
				phase[idx] += freq * args.sampleTime;
				if (phase[idx] >= 1.f)
					phase[idx] -= 1.f;

				// Compute the wave via the wave table,
				// output to the correct channel, multiplied by the output volume
				getOutput(OUTPUT_OUTPUT).setVoltage(volume_out * STS_My_Saw(phase[idx], phase_shift), idx);
			}
		}
		*/
	}

	json_t *dataToJson() override
	{
		json_t *rootJ = json_object();

		json_object_set_new(rootJ, "Band", json_integer(bandLimited));
		json_object_set_new(rootJ, "Ramp", json_integer(rampDir));
		json_object_set_new(rootJ, "Harmonics", json_integer(menu_num_Harmonics));

		return rootJ;
	}

	void dataFromJson(json_t *rootJ) override
	{
		json_t *bandLimitedJ = json_object_get(rootJ, "Band");
		json_t *rampDirJ = json_object_get(rootJ, "Ramp");
		json_t *harmonicsJ = json_object_get(rootJ, "Harmonics");

		if (bandLimitedJ)
			bandLimited = json_integer_value(bandLimitedJ);
		if (rampDirJ)
			rampDir = json_integer_value(rampDirJ);
		if (harmonicsJ)
			menu_num_Harmonics = json_integer_value(harmonicsJ);
	}
};

struct SuperZzzawWidget : ModuleWidget
{
	SuperZzzawWidget(SuperZzzaw *module)
	{
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/SuperZzzaw.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		// S1 Panel
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(8.0, 15.5)), module, SuperZzzaw::SZZ_LVL_PARAMS + 0));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(18.0, 15.5)), module, SuperZzzaw::SZZ_PHASE_PARAMS + 0));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(28.0, 15.5)), module, SuperZzzaw::SZZ_DETUNE_PARAMS + 0));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(38, 15.5)), module, SuperZzzaw::SZZ_PAN_PARAMS + 0));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.0, 26.0)), module, SuperZzzaw::SZZ_LVL_INPUTS + 0));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(18.0, 26.0)), module, SuperZzzaw::SZZ_PHASE_INPUTS + 0));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(28, 26.0)), module, SuperZzzaw::SZZ_DETUNE_INPUTS + 0));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(38, 26.0)), module, SuperZzzaw::SZZ_PAN_INPUTS + 0));

		// General In & Out
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(89.0, 110.0)), module, SuperZzzaw::PITCH_PARAM));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(99.0, 110.0)), module, SuperZzzaw::PITCH_IN_PARAM));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(108.0, 110.0)), module, SuperZzzaw::V_OCT_IN_INPUT));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(128.0, 110.0)), module, SuperZzzaw::LVL_OUT_PARAM));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(119.5, 110.0)), module, SuperZzzaw::OUTPUT_OUTPUT));
	}

	void appendContextMenu(Menu *menu) override
	{
		SuperZzzaw *module = getModule<SuperZzzaw>();

		menu->addChild(new MenuSeparator);

		menu->addChild(createIndexPtrSubmenuItem("Ramp", {"Up", "Down"}, &module->rampDir));
		menu->addChild(createIndexPtrSubmenuItem("Band", {"Unlimited", "Limited"}, &module->bandLimited));
		menu->addChild(createIndexPtrSubmenuItem("Harmonics", {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20"},
												 &module->menu_num_Harmonics));
	}
};

Model *modelSuperZzzaw = createModel<SuperZzzaw, SuperZzzawWidget>("SuperZzzaw");