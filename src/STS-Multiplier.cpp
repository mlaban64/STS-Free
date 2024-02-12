#include "plugin.hpp"
#include "sts-base.hpp"
#include <fstream>
#include <string.h>
#include <math.h>

struct Multiplier : Module {
	enum ParamId {
		PARAMS_LEN
	};
	enum InputId {
		IN_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUT1_OUTPUT,
		OUT2_OUTPUT,
		OUT3_OUTPUT,
		OUT4_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};
	
	// A simple debug message funcion. Note this takes quite a bit of CPU time, so do not expect proper sound processing when used
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

	Multiplier() 
	{
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configInput(IN_INPUT, "Voltage");
		configOutput(OUT1_OUTPUT, "Voltage 1");
		configOutput(OUT2_OUTPUT, "Voltage 2");
		configOutput(OUT3_OUTPUT, "Voltage 3");
		configOutput(OUT4_OUTPUT, "Voltage 4");
	}

	void process(const ProcessArgs& args) override 
	{
		// Get the number of polyphonic channels from the Input, if any
		int num_channels = inputs[IN_INPUT].getChannels();

		// Input not connected? Do nothing...
		if (num_channels < 1)
			return;

		// Set the number of channels as per the number of input channels
		outputs[OUT1_OUTPUT].setChannels(num_channels);
		outputs[OUT2_OUTPUT].setChannels(num_channels);
		outputs[OUT3_OUTPUT].setChannels(num_channels);
		outputs[OUT4_OUTPUT].setChannels(num_channels);

		// Copy input voltages to the four outputs
		outputs[OUT1_OUTPUT].writeVoltages(inputs[IN_INPUT].getVoltages());
		outputs[OUT2_OUTPUT].writeVoltages(inputs[IN_INPUT].getVoltages());
		outputs[OUT3_OUTPUT].writeVoltages(inputs[IN_INPUT].getVoltages());
		outputs[OUT4_OUTPUT].writeVoltages(inputs[IN_INPUT].getVoltages());
	}
};


struct MultiplierWidget : ModuleWidget {
	MultiplierWidget(Multiplier* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/STS-Multiplier.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(12.7, 20.586)), module, Multiplier::IN_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(12.7, 37.586)), module, Multiplier::OUT1_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(12.7, 54.586)), module, Multiplier::OUT2_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(12.7, 71.586)), module, Multiplier::OUT3_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(12.7, 88.586)), module, Multiplier::OUT4_OUTPUT));
	}
};


Model* modelMultiplier = createModel<Multiplier, MultiplierWidget>("Multiplier");