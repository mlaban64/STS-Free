#include "plugin.hpp"
#include "sts-base.hpp"
#include <fstream>
#include <string.h>
#include <math.h>


struct Sine_VCO : Module {
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

	// An array of values to represent the sine wave, as values in the range [-1.0, 1.0]. This can arguebly be regarded as a wavetable
	float sine_wave_lookup_table[STS_NUM_WAVE_SAMPLES];

	// local class variable declarations. For some reason, putting them as static/dynamic in the class Process() function does not work.
	// For instance, the PITCH_PARAM knob would infuence other instances of the same module. Putting it here circumvents the problem
	float pitch_param, phase_param, volume_param;
	float freq = 0.f, pitch = 0.f, phase_shift = 0.f, volume_out = 0.f;
	float freq_mod = 0.f, phase_mod = 0.f, volume_mod = 0.f;
	float freq_mod_attn = 0.f, phase_mod_attn = 0.f, volume_mod_attn = 0.f;
	int num_channels, idx;

	// Array of 16 phases to accomodate for polyphony
	float phase[16] = { 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f };

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
	float STS_My_Sine(float phase, float phase_shift)
	{
		static int idx;

		// Compute the index by mapping phase + phase_shift across the total number of samples in the wave table
		idx = (int) ((phase + phase_shift) * STS_NUM_WAVE_SAMPLES); 
		idx = idx % STS_NUM_WAVE_SAMPLES;

		return(sine_wave_lookup_table[idx]);
	}

	void InitSine_Waves ()
	{
		static int i;

		// Populate the sine wave table
		// Filled with a full sine cycle, multiplied by 5.0 to reflect the default +/- 5V audio output levels
		for (i = 0; i < STS_NUM_WAVE_SAMPLES; i++)
		{
			sine_wave_lookup_table[i] = 5.0f * std::sin(M_2PI * ((float) i / STS_NUM_WAVE_SAMPLES));
		}
	}

	Sine_VCO() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(FM_ATTN_PARAM, 0.f, 1.f, 0.f, "Attenuation for frequency modulation");
		configParam(PM_ATTN_PARAM, 0.f, 1.f, 0.f, "Attenuation for phase modulation");
		configParam(VM_ATTN_PARAM, 0.f, 1.f, 0.f, "Attenuation for volume modulation");
		configParam(PITCH_PARAM, 40.f, 10000.f, dsp::FREQ_C4, "Fixed pitch", " Hz");
		configParam(PHASE_PARAM, 0.f, 1.f, 0.f, "Phase shift", " Cycle");
		configParam(VOLUME_PARAM, 0.f, 1.f, 0.5f, "Output volume");
		configInput(V_OCT_IN_INPUT, "Pitch (V//Oct)");
		configInput(FM_IN_INPUT, "Frequence modulation");
		configInput(PM_IN_INPUT, "Phase modulation");
		configInput(VM_IN_INPUT, "Volume modulation");
		configOutput(OUTPUT_OUTPUT, "Audio Out");

		InitSine_Waves();
	}

	void process(const ProcessArgs& args) override {
		
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
			outputs[OUTPUT_OUTPUT].setVoltage(volume_out * STS_My_Sine(phase[0], phase_shift));
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
				outputs[OUTPUT_OUTPUT].setVoltage(volume_out * STS_My_Sine(phase[idx], phase_shift), idx);
			}
		}
	}
};


struct Sine_VCOWidget : ModuleWidget {
	Sine_VCOWidget(Sine_VCO* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Sine-VCO.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(18.5, 29.5)), module, Sine_VCO::FM_ATTN_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(18.5, 44.5)), module, Sine_VCO::PM_ATTN_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(18.5, 59.5)), module, Sine_VCO::VM_ATTN_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(12.7, 74.5)), module, Sine_VCO::PITCH_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(12.7, 89.5)), module, Sine_VCO::PHASE_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(12.7, 104.5)), module, Sine_VCO::VOLUME_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.0, 14.0)), module, Sine_VCO::V_OCT_IN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(6.5, 29.5)), module, Sine_VCO::FM_IN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(6.5, 44.5)), module, Sine_VCO::PM_IN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(6.5, 59.5)), module, Sine_VCO::VM_IN_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(18.5, 14.0)), module, Sine_VCO::OUTPUT_OUTPUT));
	}
};


Model* modelSine_VCO = createModel<Sine_VCO, Sine_VCOWidget>("Sine-VCO");