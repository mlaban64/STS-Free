#include "plugin.hpp"
#include "sts-base.hpp"
#include <fstream>
#include <string.h>
#include <math.h>


struct Triangle_VCO : Module {
	enum ParamId {
		FM_ATTN_PARAM,
		PM_ATTN_PARAM,
		VM_ATTN_PARAM,
		PITCH_PARAM,
		PHASE_PARAM,
		VOLUME_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		V_OCT_IN_INPUT,
		FM_IN_INPUT,
		PM_IN_INPUT,
		VM_IN_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUTPUT_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	// Some class-wide constants
	// const float M_2PI = 2.0 * M_PI;
	const float FREQ_MOD_MULTIPLIER = 0.1f;
	const float PHASE_MOD_MULTIPLIER = 0.1f;
	const float VOLUME_MOD_MULTIPLIER = 0.1f;

	#define STS_NUM_WAVE_SAMPLES 1000
	#define STS_DEF_NUM_HARMONICS 10
	
	// Some class-wide variables
	int bandLimited = 0;
	int num_Harmonics = STS_DEF_NUM_HARMONICS;
	int last_menu_num_Harmonics = STS_DEF_NUM_HARMONICS - 1;
	int menu_num_Harmonics = STS_DEF_NUM_HARMONICS - 1;
	
	// An array of values to represent the sine wave, as values in the range [-1.0, 1.0]. This can arguebly be regarded as a wavetable
	// 2 arrays are used: Band-limited (# of harmonics set as per STS_NUM_SAW_HARMONICS) and Band-unlimited, approachng the mathematicsl triangle
	float triangle_bl_wave_lookup_table[STS_NUM_WAVE_SAMPLES];
	float triangle_bu_wave_lookup_table[STS_NUM_WAVE_SAMPLES];

	// Debug function, not to be used as a logger as it kills performance
	void STS_Debug(std::string msg, float value)
	{
		std::ofstream fs;
		
		fs.open("C:/Temp/STS-Debug.txt",std::ofstream::app);

		fs << msg;
		fs << " ";
		fs << value;
		fs << "\n";
		fs.close();
    }

	// Maps  phase & phase shift to an index in the wave table
	float STS_My_Triangle(float phase, float phase_shift)
	{
		static int idx;

		// Compute the index by mapping phase + phase_shift across the total number of samples in the wave table
		idx = (int) ((phase + phase_shift) * STS_NUM_WAVE_SAMPLES); 
		idx = idx % STS_NUM_WAVE_SAMPLES;

		if (bandLimited)
			return(triangle_bl_wave_lookup_table[idx]);
		else
			return(triangle_bu_wave_lookup_table[idx]);
	}

	// Custom OnReset() to initialize wave tables and set some default values
	void onReset() override 
	{
		bandLimited = 0;
		num_Harmonics = STS_DEF_NUM_HARMONICS;
		menu_num_Harmonics = STS_DEF_NUM_HARMONICS - 1;
		last_menu_num_Harmonics = STS_DEF_NUM_HARMONICS - 1;
		InitTriangle_Waves(num_Harmonics);
	}

	void InitTriangle_Waves(int num_Harm)
	{

		static int i, j, mid, sign;
		static float iter, harmonic, h_factor, max_harmonic;

		// Populate the triangle wave table
		// Filled with a full triangle cycle, multiplied by 5.0 to reflect the default +/- 5V audio output levels
		mid = STS_NUM_WAVE_SAMPLES / 2;
		triangle_bu_wave_lookup_table[mid] = 5.0f; // set the mid point to +5V, in case of an odd number of samples
		for (i = 0; i < mid; i++)
		{
			triangle_bu_wave_lookup_table[i] = -5.0f + 10.0f * ((float) i / mid); // ramp up
			triangle_bu_wave_lookup_table[STS_NUM_WAVE_SAMPLES - i -1] = -5.0f + 10.0f * ((float) i / mid); // ramp down
		}

		// Using harmonic sine waves to mimic a band-limited triangle wave to limit aliases
		// Triangle = odd harmonics only, ¹/₉ of 3rd, ¹/₂₅ of 5th &c. (⅟harmonic²)
		for (i = 0; i < STS_NUM_WAVE_SAMPLES; i++)
		{
			iter = M_2PI * ((float) i / STS_NUM_WAVE_SAMPLES);
			triangle_bl_wave_lookup_table[i] = 0.0f;
			sign = 1;

			// Now loop through the odd harmonics until...
			for (j = 1 ; j <= num_Harmonics * 2; j+=2)
			{
				h_factor = (float) j;
				
				// Odd harmonic is added, even is subtracted
				if (sign == 1) 
					harmonic = std::sin(h_factor * iter) / (h_factor * h_factor);
				else
					harmonic = -std::sin(h_factor * iter) / (h_factor * h_factor);

				triangle_bl_wave_lookup_table[i] = triangle_bl_wave_lookup_table[i] + harmonic;
				sign = sign * -1; // flip the sign
			}
		}

		// Now fnd the max harmonic value to correct the output voltage between -5.0 and 5.0 V
		// Find tha largest harmoic first
		max_harmonic = 0.0f;
		for (i = 0; i < STS_NUM_WAVE_SAMPLES; i++)
		{
			if (triangle_bl_wave_lookup_table[i] > max_harmonic)
				max_harmonic = triangle_bl_wave_lookup_table[i];
		}
		// Then correct...
		for (i = 0; i < STS_NUM_WAVE_SAMPLES; i++)
		{
			triangle_bl_wave_lookup_table[i] *= (5.0f / max_harmonic);
		}
	}

	Triangle_VCO() 
	{
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(FM_ATTN_PARAM, 0.f, 1.f, 0.f, "Attenuation for frequency modulation");
		configParam(PM_ATTN_PARAM, 0.f, 1.f, 0.f, "Attenuation for phase modulation");
		configParam(VM_ATTN_PARAM, 0.f, 1.f, 0.f, "Attenuation for volume modulation");
		configParam(PITCH_PARAM, 40.f, 10000.f, 261.6256f, "Fixed pitch", " Hz");
		configParam(PHASE_PARAM, 0.f, 1.f, 0.f, "Phase shift", " Cycle");
		configParam(VOLUME_PARAM, 0.f, 1.f, 0.5f, "Output volume");
		configInput(V_OCT_IN_INPUT, "Pitch (V//Oct)");
		configInput(FM_IN_INPUT, "Frequence modulation");
		configInput(PM_IN_INPUT, "Phase modulation");
		configInput(VM_IN_INPUT, "Volume modulation");
		configOutput(OUTPUT_OUTPUT, "Audio");

		InitTriangle_Waves(STS_DEF_NUM_HARMONICS);
	}

	void process(const ProcessArgs& args) override {
		
		// local variable declarations
		static float pitch_param, phase_param, volume_param;
		static float freq = 0.f, pitch = 0.f, phase_shift = 0.f, volume_out = 0.f;
		static float freq_mod = 0.f, phase_mod = 0.f, volume_mod = 0.f;
		static float freq_mod_attn = 0.f, phase_mod_attn = 0.f, volume_mod_attn = 0.f;
		static int num_channels, idx;

		// Array of 16 phases to accomodate for polyphony
		static float phase[16] = { 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f };

		// Check if we need to recompute the wave tables
		if (last_menu_num_Harmonics != menu_num_Harmonics)
		{
			num_Harmonics = menu_num_Harmonics + 1;
			InitTriangle_Waves(num_Harmonics);
			last_menu_num_Harmonics = menu_num_Harmonics;
		}
		
		// Get all the values from the module UI
		pitch_param = params[PITCH_PARAM].getValue();
		phase_param = params[PHASE_PARAM].getValue();
		volume_param = params[VOLUME_PARAM].getValue();
		freq_mod_attn = params[FM_ATTN_PARAM].getValue();
		phase_mod_attn = params[PM_ATTN_PARAM].getValue();
		volume_mod_attn = params[VM_ATTN_PARAM].getValue();
		freq_mod = inputs[FM_IN_INPUT].getVoltage();
		phase_mod = inputs[PM_IN_INPUT].getVoltage();
		volume_mod = inputs[VM_IN_INPUT].getVoltage();

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
			outputs[OUTPUT_OUTPUT].setVoltage(volume_out * STS_My_Triangle(phase[0], phase_shift));
		} 
			else 
		{ 
			// Else, compute it as per the V/Oct input for each poly channel
			// Loop through all input channels
			for (idx = 0; idx < num_channels; idx++)
			{
				pitch = inputs[V_OCT_IN_INPUT].getVoltage(idx);
				freq = dsp::FREQ_C4 * std::pow(2.f, pitch);

				// Compute the pitch as per the controls
				if (inputs[FM_IN_INPUT].isConnected())
					freq = freq + freq * freq_mod * freq_mod_attn * FREQ_MOD_MULTIPLIER;

				// Accumulate the phase, make sure it rotates between 0.0 and 1.0
				phase[idx] += freq * args.sampleTime;
				if (phase[idx] >= 1.f)
					phase[idx] -= 1.f;
				
				// Compute the wave via the wave table,
				// output to the correct channel, multiplied by the output volume
				outputs[OUTPUT_OUTPUT].setVoltage(volume_out * STS_My_Triangle(phase[idx], phase_shift), idx);
			}
		}
	}
};


struct Triangle_VCOWidget : ModuleWidget {
	Triangle_VCOWidget(Triangle_VCO* module) {
		
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/STS-Triangle-VCO.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(18.5, 29.5)), module, Triangle_VCO::FM_ATTN_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(18.5, 44.5)), module, Triangle_VCO::PM_ATTN_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(18.5, 59.5)), module, Triangle_VCO::VM_ATTN_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(12.7, 74.5)), module, Triangle_VCO::PITCH_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(12.7, 89.5)), module, Triangle_VCO::PHASE_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(12.7, 104.5)), module, Triangle_VCO::VOLUME_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.0, 14.0)), module, Triangle_VCO::V_OCT_IN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(6.5, 29.5)), module, Triangle_VCO::FM_IN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(6.5, 44.5)), module, Triangle_VCO::PM_IN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(6.5, 59.5)), module, Triangle_VCO::VM_IN_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(18.5, 14.0)), module, Triangle_VCO::OUTPUT_OUTPUT));
	}
		
	void appendContextMenu(Menu* menu) override 
	{
		Triangle_VCO* module = getModule<Triangle_VCO>();

		menu->addChild(new MenuSeparator);

		menu->addChild(createIndexPtrSubmenuItem("Band", {"Unlimited", "Limited"}, &module->bandLimited));
		menu->addChild(createIndexPtrSubmenuItem("Harmonics", {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20"}, 
												&module->menu_num_Harmonics));
	}
};

Model* modelTriangle_VCO = createModel<Triangle_VCO, Triangle_VCOWidget>("Triangle-VCO");