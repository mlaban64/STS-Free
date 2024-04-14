#include "plugin.hpp"
#include "sts-base.hpp"
#include <math.h>

struct Clipper : Module
{
	enum ParamId
	{
		UTM_ATTN_PARAM,
		LTM_ATTN_PARAM,
		UPPER_THRESHOLD_PARAM,
		LOWER__THRESHOLD_PARAM,
		PARAMS_LEN
	};
	enum InputId
	{
		INPUT_INPUT,
		UTM_IN_INPUT,
		LTM_IN_INPUT,
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

	// Some class-wide parameters
	int clipMethod = 0; // 0 = Clip, 1 = Fold
#define CLIPMETHOD_CLIP 0
#define CLIPMETHOD_FOLD 1
	int polarity = 0; // 0 = Unipolar, 1 = Bipolar
#define POLARITY_UNIPOLAR 0
#define POLARITY_BIPOLAR 1

	// local class variable declarations. For some reason, putting them as static/dynamic in the class Process() function does not work.
	// For instance, the PITCH_PARAM knob would infuence other instances of the same module. Putting it here circumvents the problem
	float UTM_attn_param, LTM_attn_param, UTM_param, LTM_param;
	float UTM_in = 0.f, LTM_in = 0.f;	// Modulation input
	float UTM_mod = 0.f, LTM_mod = 0.f; // Modulation result
	float out_Volt;
	int num_channels, idx;

	Clipper()
	{
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(UTM_ATTN_PARAM, 0.f, 1.f, 0.f, "Attenuation for Upper Threshold Modulation");
		configParam(LTM_ATTN_PARAM, 0.f, 1.f, 0.f, "Attenuation for Upper Threshold Modulation");
		configParam(UPPER_THRESHOLD_PARAM, 0.f, 5.f, 5.f, "Volt");
		configParam(LOWER__THRESHOLD_PARAM, -5.f, 0.f, -5.f, "Volt");
		configInput(INPUT_INPUT, "Audio In");
		configInput(UTM_IN_INPUT, "Upper Threshold Modulation");
		configInput(LTM_IN_INPUT, "Lower Threshold Modulation");
		configOutput(OUTPUT_OUTPUT, "Audio Out");
	}

	void process(const ProcessArgs &args) override
	{
		// If no VC/In connected, bail out
		if (!inputs[INPUT_INPUT].isConnected())
			return;

		// If no Output connected, bail out
		if (!inputs[OUTPUT_OUTPUT].isConnected())
			return;

		// Get all the values from the module UI
		UTM_attn_param = params[UTM_ATTN_PARAM].getValue();
		LTM_attn_param = params[LTM_ATTN_PARAM].getValue();
		UTM_param = params[UPPER_THRESHOLD_PARAM].getValue();
		LTM_param = params[LOWER__THRESHOLD_PARAM].getValue();
		UTM_in = inputs[UTM_IN_INPUT].getVoltage();
		LTM_in = inputs[LTM_IN_INPUT].getVoltage();

		// Compute the Upper Treshold Modulation (UTM) as per the controls
		// UTM must always be > 0, LTM must always be < 0, to avoid artifacts of self-oscillating
		if (inputs[UTM_IN_INPUT].isConnected())
		{
			UTM_mod = UTM_attn_param * UTM_in + UTM_param;
			if (polarity == POLARITY_BIPOLAR)
			{
				if (UTM_mod < 0.f)
					UTM_mod = 0.f;
			}
			else
			{
				if (UTM_mod < 5.f)
					UTM_mod = 5.f;
			}
		}
		else
		{
			if (polarity == POLARITY_BIPOLAR)
				UTM_mod = UTM_param;
			else
				UTM_mod = UTM_param + 5.f;
		}

		// Compute the Lower Treshold Modulation (UTM) as per the controls
		if (inputs[LTM_IN_INPUT].isConnected())
		{
			LTM_mod = LTM_attn_param * LTM_in + LTM_param;
			if (polarity == POLARITY_BIPOLAR)
			{
				if (LTM_mod > 0.f)
					LTM_mod = 0.f;
			}
			else
			{
				if (LTM_mod > 5.f)
					LTM_mod = 5.f;
			}
		}
		else
		{
			if (polarity == POLARITY_BIPOLAR)
				LTM_mod = LTM_param;
			else
				LTM_mod = LTM_param + 5.f;
		}

		// Is the V-In connected and polyphonic?
		num_channels = inputs[INPUT_INPUT].getChannels();
		// First, match the # of output channels to the number of input channels, to ensure all other channels are reset to 0 V
		outputs[OUTPUT_OUTPUT].setChannels(num_channels);

		for (idx = 0; idx < num_channels; idx++)
		{
			// Compute the new output voltage
			out_Volt = inputs[INPUT_INPUT].getVoltage(idx);
			// output to the correct channel, multiplied by the output volume

			// If clip, set output to the threshold, else subtract from the threshold
			if (clipMethod == CLIPMETHOD_CLIP)
			{
				if (out_Volt > UTM_mod)
					out_Volt = UTM_mod;
				if (out_Volt < LTM_mod)
					out_Volt = LTM_mod;
			}
			else
			{
				if (out_Volt > UTM_mod)
					out_Volt = UTM_mod - (out_Volt - UTM_mod);
				if (out_Volt < LTM_mod)
					out_Volt = LTM_mod - (out_Volt - LTM_mod);
			}

			outputs[OUTPUT_OUTPUT].setVoltage(out_Volt, idx);
		}
		// Done, so exit
		return;
	}

	json_t *
	dataToJson() override
	{
		json_t *rootJ = json_object();

		json_object_set_new(rootJ, "Clip Method", json_integer(clipMethod));
		json_object_set_new(rootJ, "Polarity", json_integer(polarity));

		return rootJ;
	}

	void dataFromJson(json_t *rootJ) override
	{
		json_t *clipMethodJ = json_object_get(rootJ, "Clip Method");
		json_t *polarityJ = json_object_get(rootJ, "Polarity");

		if (clipMethodJ)
			clipMethod = json_integer_value(clipMethodJ);
		if (polarityJ)
			polarity = json_integer_value(polarityJ);
	}
};

struct ClipperWidget : ModuleWidget
{
	ClipperWidget(Clipper *module)
	{
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Clipper.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(18.5, 32.5)), module, Clipper::UTM_ATTN_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(18.5, 52.5)), module, Clipper::LTM_ATTN_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(12.7, 76.5)), module, Clipper::UPPER_THRESHOLD_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(12.7, 97.2)), module, Clipper::LOWER__THRESHOLD_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.0, 14.0)), module, Clipper::INPUT_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(6.5, 32.5)), module, Clipper::UTM_IN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(6.5, 52.5)), module, Clipper::LTM_IN_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(18.5, 14.0)), module, Clipper::OUTPUT_OUTPUT));
	}
	void appendContextMenu(Menu *menu) override
	{
		Clipper *module = getModule<Clipper>();

		menu->addChild(new MenuSeparator);

		menu->addChild(createIndexPtrSubmenuItem("Clip Method", {"Clip", "Fold"}, &module->clipMethod));
		menu->addChild(createIndexPtrSubmenuItem("Polarity", {"Unipolar (0..10V)", "Bipolar (-5V..5V)"}, &module->polarity));
	}
};

Model *modelClipper = createModel<Clipper, ClipperWidget>("Clipper");