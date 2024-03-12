#include "plugin.hpp"
#include "sts-base.hpp"
#include <fstream>
#include <string.h>
#include <math.h>

struct Multiplier : Module
{
	enum ParamId
	{
		PARAMS_LEN
	};
	enum InputId
	{
		IN_INPUT,
		INPUTS_LEN
	};
	enum OutputId
	{
		OUT_1_OUTPUT,
		OUT_2_OUTPUT,
		OUT_3_OUTPUT,
		OUT_4_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId
	{
		LIGHTS_LEN
	};

	// Some class-wide parameters
	int num_Channels; // Number of active channels

	// A simple debug message funcion. Note this takes quite a bit of CPU time, so do not expect proper sound processing when used
	void STS_Debug(std::string msg, float value)
	{
		std::ofstream fs;

		fs.open("C:/Temp/STS-Debug.txt", std::ofstream::app);

		fs << msg;
		fs << " ";
		fs << value;
		fs << "\n";
		fs.close();
	}

	Multiplier()
	{
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configInput(IN_INPUT, "Signal");
		configOutput(OUT_1_OUTPUT, "Signal 1");
		configOutput(OUT_2_OUTPUT, "Signal 2");
		configOutput(OUT_3_OUTPUT, "Signal 3");
		configOutput(OUT_4_OUTPUT, "Signal 4");
	}

	void process(const ProcessArgs &args) override
	{
		// Get the number of polyphonic channels from the Input, if any
		num_Channels = inputs[IN_INPUT].getChannels();

		// Input not connected? Do nothing...
		if (num_Channels < 1)
			return;

		// Set the number of channels as per the number of input channels
		outputs[OUT_1_OUTPUT].setChannels(num_Channels);
		outputs[OUT_2_OUTPUT].setChannels(num_Channels);
		outputs[OUT_3_OUTPUT].setChannels(num_Channels);
		outputs[OUT_4_OUTPUT].setChannels(num_Channels);

		// Copy input voltages to the four outputs
		outputs[OUT_1_OUTPUT].writeVoltages(inputs[IN_INPUT].getVoltages());
		outputs[OUT_2_OUTPUT].writeVoltages(inputs[IN_INPUT].getVoltages());
		outputs[OUT_3_OUTPUT].writeVoltages(inputs[IN_INPUT].getVoltages());
		outputs[OUT_4_OUTPUT].writeVoltages(inputs[IN_INPUT].getVoltages());
	}
};

struct MultiplierWidget : ModuleWidget
{
	MultiplierWidget(Multiplier *module)
	{
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Multiplier.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(12.7, 20.586)), module, Multiplier::IN_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(12.7, 37.586)), module, Multiplier::OUT_1_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(12.7, 54.586)), module, Multiplier::OUT_2_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(12.7, 71.586)), module, Multiplier::OUT_3_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(12.7, 88.586)), module, Multiplier::OUT_4_OUTPUT));
	}
};

Model *modelMultiplier = createModel<Multiplier, MultiplierWidget>("Multiplier");