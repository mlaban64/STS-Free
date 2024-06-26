#include "plugin.hpp"
#include "sts-base.hpp"
#include <math.h>

struct D_Octer : Module
{
	enum ParamId
	{
		PARAMS_LEN
	};
	enum InputId
	{
		POLY_IN_INPUT,
		INPUTS_LEN
	};
	enum OutputId
	{
		OCTm2_OUT_OUTPUT,
		OCTm1_OUT_OUTPUT,
		OCTp1_OUT_OUTPUT,
		OCTp2_OUT_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId
	{
		LIGHTS_LEN
	};

	// Some class-wide parameters
	int num_Channels, cur_Channel; // Number of active channels
	float volt_In;				   // Input voltage

	D_Octer()
	{
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configInput(POLY_IN_INPUT, "V/Oct");
		configOutput(OCTm2_OUT_OUTPUT, "2 Octaves down");
		configOutput(OCTm1_OUT_OUTPUT, "1 Octave down");
		configOutput(OCTp1_OUT_OUTPUT, "1 Octave up");
		configOutput(OCTp2_OUT_OUTPUT, "2 Octaves up");
	}

	void process(const ProcessArgs &args) override
	{

		// Get the number of channels from the Input, if any
		num_Channels = getInput(POLY_IN_INPUT).getChannels();

		// Input not connected? Do nothing...
		if (num_Channels == 0)
			return;

		// Set the number of channels as per the number of input channels
		getOutput(OCTm2_OUT_OUTPUT).setChannels(num_Channels);
		getOutput(OCTm1_OUT_OUTPUT).setChannels(num_Channels);
		getOutput(OCTp1_OUT_OUTPUT).setChannels(num_Channels);
		getOutput(OCTp2_OUT_OUTPUT).setChannels(num_Channels);

		// Now read each channel, transpose and send to output channels
		for (cur_Channel = 0; cur_Channel < num_Channels; cur_Channel++)
		{
			volt_In = getInput(POLY_IN_INPUT).getVoltage(cur_Channel);
			getOutput(OCTm2_OUT_OUTPUT).setVoltage(volt_In - 2.0, cur_Channel);
			getOutput(OCTm1_OUT_OUTPUT).setVoltage(volt_In - 1.0, cur_Channel);
			getOutput(OCTp1_OUT_OUTPUT).setVoltage(volt_In + 1.0, cur_Channel);
			getOutput(OCTp2_OUT_OUTPUT).setVoltage(volt_In + 2.0, cur_Channel);
		}
		// Done, so return
		return;
	}
};

struct D_OcterWidget : ModuleWidget
{
	D_OcterWidget(D_Octer *module)
	{
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/D-Octer.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(12.7, 20.5)), module, D_Octer::POLY_IN_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(12.7, 37.586)), module, D_Octer::OCTm2_OUT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(12.7, 54.586)), module, D_Octer::OCTm1_OUT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(12.7, 71.586)), module, D_Octer::OCTp1_OUT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(12.7, 88.586)), module, D_Octer::OCTp2_OUT_OUTPUT));
	}
};

Model *modelD_Octer = createModel<D_Octer, D_OcterWidget>("D-Octer");