#include "plugin.hpp"
#include "sts-base.hpp"
#include <fstream>
#include <string.h>
#include <math.h>


struct Splitter : Module {
	enum ParamId {
		PARAMS_LEN
	};
	enum InputId {
		POLY_IN_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		CH_1_OUT_OUTPUT,
		CH_2_OUT_OUTPUT,
		CH_3_OUT_OUTPUT,
		CH_4_OUT_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LGT_1_OUT_LIGHT,
		LGT_2_OUT_LIGHT,
		LGT_3_OUT_LIGHT,
		LGT_4_OUT_LIGHT,
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
	
	Splitter() 
	{
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configInput(POLY_IN_INPUT, "Polyphonic");
		configOutput(CH_1_OUT_OUTPUT, "Channel 1");
		configOutput(CH_2_OUT_OUTPUT, "Channel 2");
		configOutput(CH_3_OUT_OUTPUT, "Channel 3");
		configOutput(CH_4_OUT_OUTPUT, "Channel 4");
	}

	int old_num_channels = 0;

	void process(const ProcessArgs& args) override 
	{
		// local params
		float v;
		
		// Get the number of channels from the Input, if any
		int num_channels = inputs[POLY_IN_INPUT].getChannels();

		// Did the number of channels change? If so, make sure all lights are out before switching them on as per the new channel count
		// This to avoid flickering lights for no reason, every time process() is called
		if (num_channels != old_num_channels)
		{
			lights[LGT_1_OUT_LIGHT].setBrightness(0.0);
			lights[LGT_2_OUT_LIGHT].setBrightness(0.0);
			lights[LGT_3_OUT_LIGHT].setBrightness(0.0);
			lights[LGT_4_OUT_LIGHT].setBrightness(0.0);
			old_num_channels = num_channels;
		}

		// Input not connected? Do nothing...
		if (num_channels == 0)
			return;

		// Get the 1st channel voltage, copy it to Channel 1 Output, switch on the light
		v = inputs[POLY_IN_INPUT].getVoltage(0);
		outputs[CH_1_OUT_OUTPUT].setVoltage(v);
		lights[LGT_1_OUT_LIGHT].setBrightness(1.0);

		// Get the 2nd channel voltage, if it exists
		if (num_channels > 1)
		{
			v = inputs[POLY_IN_INPUT].getVoltage(1);
			outputs[CH_2_OUT_OUTPUT].setVoltage(v);
			lights[LGT_2_OUT_LIGHT].setBrightness(1.0);
		}

		// Get the 3rd channel voltage, if it exists
		if (num_channels > 2)
		{
			v = inputs[POLY_IN_INPUT].getVoltage(2);
			outputs[CH_3_OUT_OUTPUT].setVoltage(v);
			lights[LGT_3_OUT_LIGHT].setBrightness(1.0);
		}

		// Get the 4th channel voltage, if it exists
		if (num_channels > 3)
		{
			v = inputs[POLY_IN_INPUT].getVoltage(3);
			outputs[CH_4_OUT_OUTPUT].setVoltage(v);
			lights[LGT_4_OUT_LIGHT].setBrightness(1.0);
		}
	}
};


struct SplitterWidget : ModuleWidget {
	SplitterWidget(Splitter* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/STS-Splitter.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(12.7, 20.5)), module, Splitter::POLY_IN_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(12.7, 37.586)), module, Splitter::CH_1_OUT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(12.7, 54.586)), module, Splitter::CH_2_OUT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(12.7, 71.586)), module, Splitter::CH_3_OUT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(12.7, 88.586)), module, Splitter::CH_4_OUT_OUTPUT));

		addChild(createLightCentered<SmallLight<STSRedLight>>(mm2px(Vec(2.959, 37.64)), module, Splitter::LGT_1_OUT_LIGHT));
		addChild(createLightCentered<SmallLight<STSRedLight>>(mm2px(Vec(2.959, 54.64)), module, Splitter::LGT_2_OUT_LIGHT));
		addChild(createLightCentered<SmallLight<STSRedLight>>(mm2px(Vec(2.959, 71.64)), module, Splitter::LGT_3_OUT_LIGHT));
		addChild(createLightCentered<SmallLight<STSRedLight>>(mm2px(Vec(2.959, 88.64)), module, Splitter::LGT_4_OUT_LIGHT));
	}
};

Model* modelSplitter = createModel<Splitter, SplitterWidget>("Splitter");