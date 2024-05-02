#include "plugin.hpp"
#include "sts-base.hpp"
#include <math.h>

struct Pulse_VCO : Module
{
	enum ParamId
	{
		FM_ATTN_PARAM,
		PM_ATTN_PARAM,
		VM_ATTN_PARAM,
		PW_ATTN_PARAM,
		PITCH_PARAM,
		PHASE_PARAM,
		VOLUME_PARAM,
		PULSE_PARAM,
		PARAMS_LEN
	};
	enum InputId
	{
		V_OCT_IN_INPUT,
		FM_IN_INPUT,
		PM_IN_INPUT,
		VM_IN_INPUT,
		PW_IN_INPUT,
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
	const float PULSEWIDTH_MOD_MULTIPLIER = 0.1f;

#define STS_NUM_WAVE_SAMPLES 1000
#define STS_DEF_NUM_HARMONICS 10

	int bandLimited = 0;
	int num_Harmonics = STS_DEF_NUM_HARMONICS;
	int last_menu_num_Harmonics = STS_DEF_NUM_HARMONICS - 1;
	int menu_num_Harmonics = STS_DEF_NUM_HARMONICS - 1;

	// An array of values to represent the sine wave, as values in the range [-1.0, 1.0]. This can arguebly be regarded as a wavetable
	float pulse_wave_lookup_table[STS_NUM_WAVE_SAMPLES];

	// local class variable declarations
	float pitch_param, phase_param, volume_param, pulsewidth_param;
	float freq = 0.f, pitch = 0.f, phase_shift = 0.f, volume_out = 0.f, pulse_width = 0.5;
	float freq_mod = 0.f, phase_mod = 0.f, volume_mod = 0.f, pulsewidth_mod = 0.f;
	float freq_mod_attn = 0.f, phase_mod_attn = 0.f, volume_mod_attn = 0.f, pulsewidth_mod_attn = 0.f;
	int num_channels, idx;

	// Array of 16 phases to accomodate for polyphony
	float phase[16] = {0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f};

	// Maps phase & phase shift to an index in the wave table or compute a mathematical pulse
	float STS_My_Pulse(float phase, float phase_shift)
	{
		int index;
		float pulse_pos;
		// Band-unlimited?
		if (bandLimited)
		{

			// Compute the index by mapping phase + phase_shift across the total number of samples in the wave table
			index = (int)((phase + phase_shift) * STS_NUM_WAVE_SAMPLES);
			index = index % STS_NUM_WAVE_SAMPLES;

			return (pulse_wave_lookup_table[index]);
		}
		else
		{
			// Compute the pulse position by mapping phase + phase_shift across the 0..1 range with the pulse width as cut-off
			pulse_pos = phase + phase_shift;
			if (pulse_pos > 1.0f)
				pulse_pos -= 1.0f;

			if (pulse_pos < pulse_width)
				return (5.0f);
			else
				return (-5.0f);
		}
	}

	// Custom OnReset() to initialize wave tables and set some default values
	void onReset() override
	{
		bandLimited = 0;
		num_Harmonics = STS_DEF_NUM_HARMONICS;
		menu_num_Harmonics = STS_DEF_NUM_HARMONICS - 1;
		last_menu_num_Harmonics = STS_DEF_NUM_HARMONICS - 1;
		InitPulse_Waves();
	}

	void
	InitPulse_Waves()
	{
		int i, j;
		float iter, harmonic, h_factor, max_harmonic;

		// Using harmonic sine waves to mimic a band-limited square wave to limit aliases
		// Square = odd harmonics only, ¹/3 of 3rd, ¹/5 of 5th &c. (⅟harmonic)
		for (i = 0; i < STS_NUM_WAVE_SAMPLES; i++)
		{
			iter = M_2PI * ((float)i / STS_NUM_WAVE_SAMPLES);
			pulse_wave_lookup_table[i] = 0.0f;

			// Now loop through the odd harmonics until...
			for (j = 1; j <= num_Harmonics * 2; j += 2)
			{
				h_factor = (float)j;
				harmonic = std::sin(h_factor * iter) / (h_factor);
				pulse_wave_lookup_table[i] = pulse_wave_lookup_table[i] + harmonic;
			}
		}

		// Now fnd the max harmonic value to correct the output voltage between -5.0 and 5.0 V
		// Find tha largest harmoic first
		max_harmonic = 0.0f;
		for (i = 0; i < STS_NUM_WAVE_SAMPLES; i++)
		{
			if (pulse_wave_lookup_table[i] > max_harmonic)
				max_harmonic = pulse_wave_lookup_table[i];
		}
		// Then correct...
		for (i = 0; i < STS_NUM_WAVE_SAMPLES; i++)
		{
			pulse_wave_lookup_table[i] *= (5.0f / max_harmonic);
		}
	}

	Pulse_VCO()
	{
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(FM_ATTN_PARAM, 0.f, 1.f, 0.f, "Attenuation for frequency modulation");
		configParam(PM_ATTN_PARAM, 0.f, 1.f, 0.f, "Attenuation for phase modulation");
		configParam(VM_ATTN_PARAM, 0.f, 1.f, 0.f, "Attenuation for volume modulation");
		configParam(PW_ATTN_PARAM, 0.f, 1.f, 0.f, "Attenuation for pulse-width modulation");
		configParam(PITCH_PARAM, 10.f, 20000.f, dsp::FREQ_C4, "Fixed pitch", " Hz");
		configParam(PHASE_PARAM, 0.f, 1.f, 0.f, "Phase shift", " Cycle");
		configParam(VOLUME_PARAM, 0.f, 1.f, 0.5f, "Output volume");
		configParam(PULSE_PARAM, 1.f, 99.f, 50.0f, "Pulse Width", " %");
		configInput(V_OCT_IN_INPUT, "Pitch (V//Oct)");
		configInput(FM_IN_INPUT, "Frequence modulation");
		configInput(PM_IN_INPUT, "Phase modulation");
		configInput(VM_IN_INPUT, "Volume modulation");
		configInput(PW_IN_INPUT, "Pulse-width modulation");
		configOutput(OUTPUT_OUTPUT, "Audio Out");

		InitPulse_Waves();
	}

	void process(const ProcessArgs &args) override
	{
		// Check if we need to recompute the wave tables
		if (last_menu_num_Harmonics != menu_num_Harmonics)
		{
			num_Harmonics = menu_num_Harmonics + 1;
			InitPulse_Waves();
			last_menu_num_Harmonics = menu_num_Harmonics;
		}

		// Get all the values from the module UI
		pitch_param = params[PITCH_PARAM].getValue();
		phase_param = params[PHASE_PARAM].getValue();
		volume_param = params[VOLUME_PARAM].getValue();
		pulsewidth_param = params[PULSE_PARAM].getValue();
		freq_mod_attn = params[FM_ATTN_PARAM].getValue();
		phase_mod_attn = params[PM_ATTN_PARAM].getValue();
		volume_mod_attn = params[VM_ATTN_PARAM].getValue();
		pulsewidth_mod_attn = params[PW_ATTN_PARAM].getValue();
		freq_mod = inputs[FM_IN_INPUT].getVoltage();
		phase_mod = inputs[PM_IN_INPUT].getVoltage();
		volume_mod = inputs[VM_IN_INPUT].getVoltage();
		pulsewidth_mod = inputs[PW_IN_INPUT].getVoltage();

		// Compute the volume output as per the controls
		if (inputs[VM_IN_INPUT].isConnected())
			volume_out = volume_param + volume_mod * volume_mod_attn * VOLUME_MOD_MULTIPLIER;
		else
			volume_out = volume_param;

		// Compute the phase shift as per the controls. Make sure it is not negative, as this may happen with modulation with a bipolar signal
		if (inputs[PM_IN_INPUT].isConnected())
		{
			phase_shift = phase_param + phase_mod * phase_mod_attn * PHASE_MOD_MULTIPLIER;
			if (phase_shift < 0.0f)
				phase_shift += 1.0f;
		}
		else
			phase_shift = phase_param;

		// Compute the pulse width output as per the controls
		if (inputs[PW_IN_INPUT].isConnected())
			pulse_width = 0.01f * pulsewidth_param + pulsewidth_mod * pulsewidth_mod_attn * PULSEWIDTH_MOD_MULTIPLIER;
		else
			pulse_width = 0.01f * pulsewidth_param; // pulse width param is a %

		// Is the V-In connected?
		num_channels = inputs[V_OCT_IN_INPUT].getChannels();
		// First, match the # of output channels to the number of input channels, to ensure all other channels are reset to 0 V
		outputs[OUTPUT_OUTPUT].setChannels(num_channels);

		if (num_channels == 0)
		// If not, set the frequency as per the pitch parameter, using phase[0]
		{
			// Compute the pitch as per the controls
			if (inputs[FM_IN_INPUT].isConnected())
				freq = pitch_param + pitch_param * freq_mod * freq_mod_attn * FREQ_MOD_MULTIPLIER;
			else
				freq = pitch_param;

			// Accumulate the phase, make sure it rotates between 0.0 and 1.0
			phase[0] += freq * args.sampleTime;
			if (phase[0] >= 1.f)
				phase[0] -= 1.f;

			// Compute the wave via the wave table,
			// output to the correct channel, multiplied by the output volume
			outputs[OUTPUT_OUTPUT].setVoltage(volume_out * STS_My_Pulse(phase[0], phase_shift));
		}
		else
		{
			// Else, compute it as per the V/Oct input for each poly channel
			// Loop through all input channels
			for (idx = 0; idx < num_channels; idx++)
			{
				pitch = inputs[V_OCT_IN_INPUT].getVoltage(idx);
				freq = pitch_param * std::pow(2.f, pitch);

				// Compute the pitch as per the controls
				if (inputs[FM_IN_INPUT].isConnected())
					freq = freq + freq * freq_mod * freq_mod_attn * FREQ_MOD_MULTIPLIER;

				// Accumulate the phase, make sure it rotates between 0.0 and 1.0
				phase[idx] += freq * args.sampleTime;
				if (phase[idx] >= 1.f)
					phase[idx] -= 1.f;

				// Compute the wave via the wave table,
				// output to the correct channel, multiplied by the output volume
				outputs[OUTPUT_OUTPUT].setVoltage(volume_out * STS_My_Pulse(phase[idx], phase_shift), idx);
			}
		}
	}

	json_t *dataToJson() override
	{
		json_t *rootJ = json_object();

		json_object_set_new(rootJ, "Band", json_integer(bandLimited));
		json_object_set_new(rootJ, "Harmonics", json_integer(menu_num_Harmonics));

		return rootJ;
	}

	void dataFromJson(json_t *rootJ) override
	{
		json_t *bandLimitedJ = json_object_get(rootJ, "Band");
		json_t *harmonicsJ = json_object_get(rootJ, "Harmonics");

		if (bandLimitedJ)
			bandLimited = json_integer_value(bandLimitedJ);
		if (harmonicsJ)
			menu_num_Harmonics = json_integer_value(harmonicsJ);
	}
};

struct Pulse_VCOWidget : ModuleWidget
{
	Pulse_VCOWidget(Pulse_VCO *module)
	{
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Pulse-VCO.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(18.5, 28.5)), module, Pulse_VCO::FM_ATTN_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(18.5, 40.0)), module, Pulse_VCO::PM_ATTN_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(18.5, 51.5)), module, Pulse_VCO::VM_ATTN_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(18.5, 63.0)), module, Pulse_VCO::PW_ATTN_PARAM));

		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(7.5, 77.5)), module, Pulse_VCO::PITCH_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(17.5, 87.5)), module, Pulse_VCO::PHASE_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(7.5, 97.5)), module, Pulse_VCO::VOLUME_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(17.5, 107.5)), module, Pulse_VCO::PULSE_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.0, 14.0)), module, Pulse_VCO::V_OCT_IN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(6.5, 28.5)), module, Pulse_VCO::FM_IN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(6.5, 40.0)), module, Pulse_VCO::PM_IN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(6.5, 51.5)), module, Pulse_VCO::VM_IN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(6.5, 63.0)), module, Pulse_VCO::PW_IN_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(18.5, 14.0)), module, Pulse_VCO::OUTPUT_OUTPUT));
	}

	void appendContextMenu(Menu *menu) override
	{
		Pulse_VCO *module = getModule<Pulse_VCO>();

		menu->addChild(new MenuSeparator);

		menu->addChild(createIndexPtrSubmenuItem("Band", {"Unlimited", "Limited"}, &module->bandLimited));
		menu->addChild(createIndexPtrSubmenuItem("Harmonics", {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20"},
												 &module->menu_num_Harmonics));
	}
};

Model *modelPulse_VCO = createModel<Pulse_VCO, Pulse_VCOWidget>("Pulse-VCO");