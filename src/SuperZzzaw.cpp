#include "plugin.hpp"
#include "sts-base.hpp"
#include <math.h>

struct SuperZzzaw : Module
{

// Some constants
#define STS_NUM_WAVE_SAMPLES 1000
#define STS_DEF_NUM_HARMONICS 10
#define STS_NUM_VCOS 12

	enum ParamId
	{
		ENUMS(SZZ_LVL_PARAMS, STS_NUM_VCOS),
		ENUMS(SZZ_PHASE_PARAMS, STS_NUM_VCOS),
		ENUMS(SZZ_DETUNE_PARAMS, STS_NUM_VCOS),
		ENUMS(SZZ_PAN_PARAMS, STS_NUM_VCOS),
		PITCH_PARAM,
		LVL_OUT_PARAM,
		PARAMS_LEN
	};
	enum InputId
	{
		ENUMS(SZZ_LVL_INPUTS, STS_NUM_VCOS),
		ENUMS(SZZ_PHASE_INPUTS, STS_NUM_VCOS),
		ENUMS(SZZ_DETUNE_INPUTS, STS_NUM_VCOS),
		ENUMS(SZZ_PAN_INPUTS, STS_NUM_VCOS),
		PITCH_IN_INPUT,
		V_OCT_IN_INPUT,
		INPUTS_LEN
	};
	enum OutputId
	{
		LEFT_OUT_OUTPUT,
		RIGHT_OUT_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId
	{
		LIGHTS_LEN
	};

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

	// local class variable declarations to hold data for each VCO
	float szz_Level[STS_NUM_VCOS] = {};																	// Level for each VCO
	float szz_Phase[STS_NUM_VCOS] = {};																	// Phase for each VCO
	float szz_Detune[STS_NUM_VCOS] = {};																// Detune for each VCO
	float szz_Pan[STS_NUM_VCOS] = {};																	// Pan for each VCO
	float mono_Phase[STS_NUM_VCOS] = {};																// Phase per VCO in case of monophonic (disconnected VCO_IN)
	float szz_Out[2][STS_NUM_VCOS] = {};																// Stereo output for each VCO for each channel
	int mapped_Harmonics[20] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 12, 14, 16, 18, 20, 25, 30, 35, 40, 50}; // Mapping menu number to value

	bool szz_Stereo = false;

	// Array of 16 phases to accomodate for polyphony, one for each VCO, as each VCO has a detune/phase that needs to be administered separately
	float channel_phase[16][STS_NUM_VCOS] = {};

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
		for (i = 0; i < STS_NUM_VCOS; i++)
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
			configParam(SZZ_DETUNE_PARAMS + i, -5.f, 5.f, 0.f, fmt);

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
		configInput(PITCH_IN_INPUT, "Pitch Modulation");
		configParam(LVL_OUT_PARAM, 0.f, 1.f, 0.5f, "Ouput Level");
		configOutput(LEFT_OUT_OUTPUT, "Left/Mono Audio Out");
		configOutput(RIGHT_OUT_OUTPUT, "Right Audio Out");

		InitSaw_Waves(STS_DEF_NUM_HARMONICS);
	}

	void process(const ProcessArgs &args) override
	{
		int i = 0, channel = 0, num_channels = 0; // used to loop through harmonics & polyphonic channels
		float left_Out, right_Out;				  // temp output for looping through VCO's
		float pitch_param = 0.f;				  // Pitch parameter
		float level_param = 0.f;				  // Level Out parameter
		float chan_voltage = 0.f;				  // Input voltage for the channel
		float freq = 0.f;						  // Frequency
		float pan = 0.f;						  // temp pan value
		float saw_wave = 0.f;					  // temp saw wave value

		// Check if we need to recompute the wave tables
		if (last_menu_num_Harmonics != menu_num_Harmonics)
		{
			num_Harmonics = mapped_Harmonics[menu_num_Harmonics];
			InitSaw_Waves(num_Harmonics);
			last_menu_num_Harmonics = menu_num_Harmonics;
		}

		// Get all the values from thebasic module UI
		level_param = getParam(LVL_OUT_PARAM).getValue();
		pitch_param = getParam(PITCH_PARAM).getValue();

		// Recompute array values
		for (i = 0; i < STS_NUM_VCOS; i++)
		{
			// Compute the level modulation as per the controls.
			if (getInput(SZZ_LVL_INPUTS + i).isConnected())
				szz_Level[i] = 0.1f * getInput(SZZ_LVL_INPUTS + i).getVoltage();
			else
				szz_Level[i] = getParam(SZZ_LVL_PARAMS + i).getValue();

			// Compute the phase shift modulation as per the controls.
			if (getInput(SZZ_PHASE_INPUTS + i).isConnected())
				szz_Phase[i] = abs(0.1f * getInput(SZZ_PHASE_INPUTS + i).getVoltage()) + getParam(SZZ_PHASE_PARAMS + i).getValue();
			else
				szz_Phase[i] = getParam(SZZ_PHASE_PARAMS + i).getValue();

			// Compute the detune as per the controls.
			if (getInput(SZZ_DETUNE_INPUTS + i).isConnected())
				szz_Detune[i] = 0.01f * getInput(SZZ_DETUNE_INPUTS + i).getVoltage();
			else
				szz_Detune[i] = 0.01f * getParam(SZZ_DETUNE_PARAMS + i).getValue();

			// Compute the panning as per the controls.
			if (getInput(SZZ_PAN_INPUTS + i).isConnected())
				szz_Pan[i] = 0.1f * getInput(SZZ_PAN_INPUTS + i).getVoltage();
			else
				szz_Pan[i] = getParam(SZZ_PAN_PARAMS + i).getValue();
		}

		// Is the V-In connected?
		num_channels = getInput(V_OCT_IN_INPUT).getChannels();

		// First, match the # of output channels to the number of input channels, to ensure all other channels are reset to 0 V in case of a change in polyphony
		getOutput(LEFT_OUT_OUTPUT).setChannels(num_channels);
		getOutput(RIGHT_OUT_OUTPUT).setChannels(num_channels);

		// Is only left Out connected? Then mono, else assume stereo
		// Right-only connected results in mono, but then no sound, as left is the mono channel
		if (getOutput(LEFT_OUT_OUTPUT).isConnected() && getOutput(RIGHT_OUT_OUTPUT).isConnected())
			szz_Stereo = true;
		else
			szz_Stereo = false;

		// If no VC-In, use the fixed pitch parameter for each VCO
		// Use szz_OutP[][][0] for storing the intermediate VCO wave values
		if (num_channels == 0)
		{
			if (getInput(PITCH_IN_INPUT).isConnected())
				freq = pitch_param + 0.1f * pitch_param * getInput(PITCH_IN_INPUT).getVoltage();
			else
				freq = pitch_param;

			// limit the pitch if modulation takes it too extreme
			if (freq < 10.f)
				freq = 10.f;
			else if (freq > 20000.f)
				freq = 20000.f;

			// Loop through all VCO's
			for (i = 0; i < STS_NUM_VCOS; i++)
			{
				// Process only if level != 0.0, as only then it contributes
				if (szz_Level[i] != 0.f)
				{
					// Accumulate the phase, make sure it rotates between 0.0 and 1.0
					mono_Phase[i] += (freq + freq * szz_Detune[i]) * args.sampleTime;
					if (mono_Phase[i] >= 1.f)
						mono_Phase[i] -= 1.f;

					// Now compute the output
					if (szz_Stereo)
					{
						float pan = 0.f, saw_wave = 0.f;

						// Map panning to 0..1
						pan = (1.f + szz_Pan[i]) * 0.5f;
						saw_wave = level_param * szz_Level[i] * STS_My_Saw(mono_Phase[i], szz_Phase[i]);
						szz_Out[0][i] = (1.0 - pan) * saw_wave;
						szz_Out[1][i] = pan * saw_wave;
					}
					else
						szz_Out[0][i] = szz_Out[1][i] = level_param * szz_Level[i] * STS_My_Saw(mono_Phase[i], szz_Phase[i]);
				}
				// If Level = 0.0, make sure this VCO does not contribute
				else
					szz_Out[0][i] = szz_Out[1][i] = 0.f;
			}

			left_Out = right_Out = 0.f;
			for (i = 0; i < STS_NUM_VCOS; i++)
			{
				left_Out += szz_Out[0][i];
				right_Out += szz_Out[1][i];
			}

			// Compute the wave via the wave table,
			// output to the correct channel, multiplied by the output volume
			if (szz_Stereo)
			{
				getOutput(LEFT_OUT_OUTPUT).setVoltage(left_Out);
				getOutput(RIGHT_OUT_OUTPUT).setVoltage(right_Out);
			}
			else
			{
				getOutput(LEFT_OUT_OUTPUT).setVoltage(left_Out + right_Out);
				getOutput(RIGHT_OUT_OUTPUT).setVoltage(0);
			}
		}
		else
		{
			// Else, compute it as per the V/Oct input for each poly channel
			// Loop through each poly channel
			for (channel = 0; channel < num_channels; channel++)
			{
				chan_voltage = getInput(V_OCT_IN_INPUT).getVoltage(channel);
				freq = pitch_param * std::pow(2.f, chan_voltage);

				// Compute the pitch as per the controls
				if (getInput(PITCH_IN_INPUT).isConnected())
					// Future
					freq = freq + freq * getInput(PITCH_IN_INPUT).getVoltage();

				// limit the pitch if modulation takes it too extreme
				if (freq < 10.f)
					freq = 10.f;
				else if (freq > 20000.f)
					freq = 20000.f;

				// Loop through all VCO's
				for (i = 0; i < STS_NUM_VCOS; i++)
				{
					// Accumulate the phase for this VCO, make sure it rotates between 0.0 and 1.0
					// Note that you have to do this, even if the VCO has no contrubution at this point
					channel_phase[channel][i] += (freq + freq * szz_Detune[i]) * args.sampleTime;
					if (channel_phase[channel][i] >= 1.f)
						channel_phase[channel][i] -= 1.f;

					// If this VCO has a level != 0.0, then process it, else ignore
					if (szz_Level[i] != 0.f)
					{
						// Compute the combined output for the channel
						saw_wave = level_param * szz_Level[i] * STS_My_Saw(channel_phase[channel][i], szz_Phase[i]);
						if (szz_Stereo)
						{
							// Map panning to 0..1
							pan = (1.f + szz_Pan[i]) * 0.5f;
							szz_Out[0][i] = (1.0 - pan) * saw_wave;
							szz_Out[1][i] = pan * saw_wave;
						}
						else
							szz_Out[0][i] = szz_Out[1][i] = saw_wave;
					}
					else
						// Set out to zero as starting point for this VCO
						szz_Out[0][i] = szz_Out[1][i] = 0.f;
				}

				// Now compute the total output for the channel
				left_Out = right_Out = 0.f;
				for (i = 0; i < STS_NUM_VCOS; i++)
				{
					left_Out += szz_Out[0][i];
					right_Out += szz_Out[1][i];
				}

				if (szz_Stereo)
				{
					getOutput(LEFT_OUT_OUTPUT).setVoltage(left_Out, channel);
					getOutput(RIGHT_OUT_OUTPUT).setVoltage(right_Out, channel);
				}
				else
				{
					getOutput(LEFT_OUT_OUTPUT).setVoltage((left_Out + right_Out), channel);
					getOutput(RIGHT_OUT_OUTPUT).setVoltage(0.f, channel);
				}
			}
		}
	}

	json_t *
	dataToJson() override
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
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(8.556, 15.171)), module, SuperZzzaw::SZZ_LVL_PARAMS + 0));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(18.556, 15.171)), module, SuperZzzaw::SZZ_PHASE_PARAMS + 0));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(28.556, 15.171)), module, SuperZzzaw::SZZ_DETUNE_PARAMS + 0));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(38.556, 15.171)), module, SuperZzzaw::SZZ_PAN_PARAMS + 0));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.556, 25.671)), module, SuperZzzaw::SZZ_LVL_INPUTS + 0));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(18.556, 25.671)), module, SuperZzzaw::SZZ_PHASE_INPUTS + 0));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(28.556, 25.671)), module, SuperZzzaw::SZZ_DETUNE_INPUTS + 0));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(38.556, 25.671)), module, SuperZzzaw::SZZ_PAN_INPUTS + 0));

		// S2 Panel
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(54.162, 15.171)), module, SuperZzzaw::SZZ_LVL_PARAMS + 1));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(64.162, 15.171)), module, SuperZzzaw::SZZ_PHASE_PARAMS + 1));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(74.162, 15.171)), module, SuperZzzaw::SZZ_DETUNE_PARAMS + 1));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(84.162, 15.171)), module, SuperZzzaw::SZZ_PAN_PARAMS + 1));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(54.162, 25.671)), module, SuperZzzaw::SZZ_LVL_INPUTS + 1));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(64.162, 25.671)), module, SuperZzzaw::SZZ_PHASE_INPUTS + 1));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(74.162, 25.671)), module, SuperZzzaw::SZZ_DETUNE_INPUTS + 1));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(84.162, 25.671)), module, SuperZzzaw::SZZ_PAN_INPUTS + 1));

		// S3 Panel
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(99.162, 15.171)), module, SuperZzzaw::SZZ_LVL_PARAMS + 2));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(109.162, 15.171)), module, SuperZzzaw::SZZ_PHASE_PARAMS + 2));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(119.162, 15.171)), module, SuperZzzaw::SZZ_DETUNE_PARAMS + 2));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(129.162, 15.171)), module, SuperZzzaw::SZZ_PAN_PARAMS + 2));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(99.162, 25.671)), module, SuperZzzaw::SZZ_LVL_INPUTS + 2));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(109.162, 25.671)), module, SuperZzzaw::SZZ_PHASE_INPUTS + 2));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(119.162, 25.671)), module, SuperZzzaw::SZZ_DETUNE_INPUTS + 2));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(129.162, 25.671)), module, SuperZzzaw::SZZ_PAN_INPUTS + 2));

		// S4 Panel
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(8.556, 39.171)), module, SuperZzzaw::SZZ_LVL_PARAMS + 3));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(18.556, 39.171)), module, SuperZzzaw::SZZ_PHASE_PARAMS + 3));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(28.556, 39.171)), module, SuperZzzaw::SZZ_DETUNE_PARAMS + 3));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(38.556, 39.171)), module, SuperZzzaw::SZZ_PAN_PARAMS + 3));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.556, 49.671)), module, SuperZzzaw::SZZ_LVL_INPUTS + 3));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(18.556, 49.671)), module, SuperZzzaw::SZZ_PHASE_INPUTS + 3));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(28.556, 49.671)), module, SuperZzzaw::SZZ_DETUNE_INPUTS + 3));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(38.556, 49.671)), module, SuperZzzaw::SZZ_PAN_INPUTS + 3));

		// S5 Panel
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(54.162, 39.171)), module, SuperZzzaw::SZZ_LVL_PARAMS + 4));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(64.162, 39.171)), module, SuperZzzaw::SZZ_PHASE_PARAMS + 4));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(74.162, 39.171)), module, SuperZzzaw::SZZ_DETUNE_PARAMS + 4));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(84.162, 39.171)), module, SuperZzzaw::SZZ_PAN_PARAMS + 4));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(54.162, 49.671)), module, SuperZzzaw::SZZ_LVL_INPUTS + 4));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(64.162, 49.671)), module, SuperZzzaw::SZZ_PHASE_INPUTS + 4));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(74.162, 49.671)), module, SuperZzzaw::SZZ_DETUNE_INPUTS + 4));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(84.162, 49.671)), module, SuperZzzaw::SZZ_PAN_INPUTS + 4));

		// S6 Panel
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(99.162, 39.171)), module, SuperZzzaw::SZZ_LVL_PARAMS + 5));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(109.162, 39.171)), module, SuperZzzaw::SZZ_PHASE_PARAMS + 5));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(119.162, 39.171)), module, SuperZzzaw::SZZ_DETUNE_PARAMS + 5));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(129.162, 39.171)), module, SuperZzzaw::SZZ_PAN_PARAMS + 5));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(99.162, 49.671)), module, SuperZzzaw::SZZ_LVL_INPUTS + 5));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(109.162, 49.671)), module, SuperZzzaw::SZZ_PHASE_INPUTS + 5));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(119.162, 49.671)), module, SuperZzzaw::SZZ_DETUNE_INPUTS + 5));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(129.162, 49.671)), module, SuperZzzaw::SZZ_PAN_INPUTS + 5));

		// S7 Panel
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(8.556, 63.171)), module, SuperZzzaw::SZZ_LVL_PARAMS + 6));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(18.556, 63.171)), module, SuperZzzaw::SZZ_PHASE_PARAMS + 6));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(28.556, 63.171)), module, SuperZzzaw::SZZ_DETUNE_PARAMS + 6));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(38.556, 63.171)), module, SuperZzzaw::SZZ_PAN_PARAMS + 6));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.556, 73.671)), module, SuperZzzaw::SZZ_LVL_INPUTS + 6));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(18.556, 73.671)), module, SuperZzzaw::SZZ_PHASE_INPUTS + 6));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(28.556, 73.671)), module, SuperZzzaw::SZZ_DETUNE_INPUTS + 6));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(38.556, 73.671)), module, SuperZzzaw::SZZ_PAN_INPUTS + 6));

		// S8 Panel
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(54.162, 63.171)), module, SuperZzzaw::SZZ_LVL_PARAMS + 7));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(64.162, 63.171)), module, SuperZzzaw::SZZ_PHASE_PARAMS + 7));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(74.162, 63.171)), module, SuperZzzaw::SZZ_DETUNE_PARAMS + 7));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(84.162, 63.171)), module, SuperZzzaw::SZZ_PAN_PARAMS + 7));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(54.162, 73.671)), module, SuperZzzaw::SZZ_LVL_INPUTS + 7));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(64.162, 73.671)), module, SuperZzzaw::SZZ_PHASE_INPUTS + 7));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(74.162, 73.671)), module, SuperZzzaw::SZZ_DETUNE_INPUTS + 7));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(84.162, 73.671)), module, SuperZzzaw::SZZ_PAN_INPUTS + 7));

		// S9 Panel
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(99.162, 63.171)), module, SuperZzzaw::SZZ_LVL_PARAMS + 8));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(109.162, 63.171)), module, SuperZzzaw::SZZ_PHASE_PARAMS + 8));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(119.162, 63.171)), module, SuperZzzaw::SZZ_DETUNE_PARAMS + 8));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(129.162, 63.171)), module, SuperZzzaw::SZZ_PAN_PARAMS + 8));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(99.162, 73.671)), module, SuperZzzaw::SZZ_LVL_INPUTS + 8));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(109.162, 73.671)), module, SuperZzzaw::SZZ_PHASE_INPUTS + 8));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(119.162, 73.671)), module, SuperZzzaw::SZZ_DETUNE_INPUTS + 8));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(129.162, 73.671)), module, SuperZzzaw::SZZ_PAN_INPUTS + 8));

		// S10 Panel
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(8.556, 87.171)), module, SuperZzzaw::SZZ_LVL_PARAMS + 9));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(18.556, 87.171)), module, SuperZzzaw::SZZ_PHASE_PARAMS + 9));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(28.556, 87.171)), module, SuperZzzaw::SZZ_DETUNE_PARAMS + 9));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(38.556, 87.171)), module, SuperZzzaw::SZZ_PAN_PARAMS + 9));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.556, 97.671)), module, SuperZzzaw::SZZ_LVL_INPUTS + 9));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(18.556, 97.671)), module, SuperZzzaw::SZZ_PHASE_INPUTS + 9));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(28.556, 97.671)), module, SuperZzzaw::SZZ_DETUNE_INPUTS + 9));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(38.556, 97.671)), module, SuperZzzaw::SZZ_PAN_INPUTS + 9));

		// S11 Panel
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(54.162, 87.171)), module, SuperZzzaw::SZZ_LVL_PARAMS + 10));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(64.162, 87.171)), module, SuperZzzaw::SZZ_PHASE_PARAMS + 10));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(74.162, 87.171)), module, SuperZzzaw::SZZ_DETUNE_PARAMS + 10));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(84.162, 87.171)), module, SuperZzzaw::SZZ_PAN_PARAMS + 10));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(54.162, 97.671)), module, SuperZzzaw::SZZ_LVL_INPUTS + 10));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(64.162, 97.671)), module, SuperZzzaw::SZZ_PHASE_INPUTS + 10));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(74.162, 97.671)), module, SuperZzzaw::SZZ_DETUNE_INPUTS + 10));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(84.162, 97.671)), module, SuperZzzaw::SZZ_PAN_INPUTS + 10));

		// S12 Panel
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(99.162, 87.171)), module, SuperZzzaw::SZZ_LVL_PARAMS + 11));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(109.162, 87.171)), module, SuperZzzaw::SZZ_PHASE_PARAMS + 11));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(119.162, 87.171)), module, SuperZzzaw::SZZ_DETUNE_PARAMS + 11));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(129.162, 87.171)), module, SuperZzzaw::SZZ_PAN_PARAMS + 11));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(99.162, 97.671)), module, SuperZzzaw::SZZ_LVL_INPUTS + 11));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(109.162, 97.671)), module, SuperZzzaw::SZZ_PHASE_INPUTS + 11));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(119.162, 97.671)), module, SuperZzzaw::SZZ_DETUNE_INPUTS + 11));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(129.162, 97.671)), module, SuperZzzaw::SZZ_PAN_INPUTS + 11));

		// General In & Out
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(79.0, 110.0)), module, SuperZzzaw::PITCH_PARAM));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(89.0, 110.0)), module, SuperZzzaw::PITCH_IN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(99.0, 110.0)), module, SuperZzzaw::V_OCT_IN_INPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(109.5, 110.0)), module, SuperZzzaw::LEFT_OUT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(117.5, 110.0)), module, SuperZzzaw::RIGHT_OUT_OUTPUT));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(128.0, 110.0)), module, SuperZzzaw::LVL_OUT_PARAM));
	}

	void appendContextMenu(Menu *menu) override
	{

		SuperZzzaw *module = getModule<SuperZzzaw>();

		menu->addChild(new MenuSeparator);

		menu->addChild(createIndexPtrSubmenuItem("Ramp", {"Up", "Down"}, &module->rampDir));
		menu->addChild(createIndexPtrSubmenuItem("Band", {"Unlimited", "Limited"}, &module->bandLimited));
		menu->addChild(createIndexPtrSubmenuItem("Harmonics", {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "12", "14", "16", "18", "20", "25", "30", "35", "40", "50"},
												 &module->menu_num_Harmonics));
	}
};

Model *modelSuperZzzaw = createModel<SuperZzzaw, SuperZzzawWidget>("SuperZzzaw");