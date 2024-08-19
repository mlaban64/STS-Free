#include "plugin.hpp"
#include "sts-base.hpp"
#include <math.h>

struct Harmoblender : Module
{
	enum ParamId
	{
		H1_LVL_PARAM,
		H1_PHASE_PARAM,
		H1_MULT_PARAM,
		PARAMS_LEN
	};
	enum InputId
	{
		V_OCT_IN_INPUT,
		H1_LVL_IN_INPUT,
		H1_PHASE_IN_INPUT,
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

	// An array of values to represent the sine wave, as values in the range [-1.0, 1.0]. This can arguebly be regarded as a wavetable
	float sine_wave_lookup_table[STS_NUM_WAVE_SAMPLES];

	// local class variable declarations
	float volume_param;
	float freq = 0.f, pitch = 0.f, phase_shift = 0.f, volume_out = 0.f;
	float volume_mod = 0.f;
	int num_channels, idx;

	// Array of 16 phases to accomodate for polyphony
	float phase[16] = {0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f};

	// Maps  phase & phase shift to an index in the wave table
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
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(H1_LVL_PARAM, -1.f, 1.f, 0.f, "Harmonic 1 Level");
		configParam(H1_PHASE_PARAM, -0.5f, 0.5f, 0.f, "Harmonic 1 Phase");
		configParam(H1_MULT_PARAM, 0.f, 20.f, 1.f, "Harmonic 1 Multiplcation");
		getParamQuantity(H1_MULT_PARAM)->snapEnabled = true;

		configInput(V_OCT_IN_INPUT, "Pitch (V//Oct)");
		configInput(H1_LVL_IN_INPUT, "Level (V//Oct)");
		configInput(H1_PHASE_IN_INPUT, "Phase (V//Oct)");

		configOutput(OUTPUT_OUTPUT, "Audio");

		InitSine_Waves();
	}

	void process(const ProcessArgs &args) override
	{

		// Get all the values from the module UI

		/*
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
					getOutput(OUTPUT_OUTPUT).setVoltage(volume_out * STS_My_Sine(phase[0], phase_shift));
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
						getOutput(OUTPUT_OUTPUT).setVoltage(volume_out * STS_My_Sine(phase[idx], phase_shift), idx);
					}
				}
				*/
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

		// All Params
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(8.0, 15.5)), module, Harmoblender::H1_LVL_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(18.0, 15.5)), module, Harmoblender::H1_PHASE_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(28.0, 15.5)), module, Harmoblender::H1_MULT_PARAM));
		// All Inputs
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(107.543, 106.169)), module, Harmoblender::V_OCT_IN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.0, 26.0)), module, Harmoblender::H1_LVL_IN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(18.0, 26.0)), module, Harmoblender::H1_PHASE_IN_INPUT));
		// All Outputs
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(119.043, 106.169)), module, Harmoblender::OUTPUT_OUTPUT));
	}
};

Model *modelHarmoblender = createModel<Harmoblender, HarmoblenderWidget>("Harmoblender");