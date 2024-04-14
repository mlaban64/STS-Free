#include "plugin.hpp"
#include "sts-base.hpp"
#include <math.h>

struct Splitter : Module
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
		CH_1_OUT_OUTPUT,
		CH_2_OUT_OUTPUT,
		CH_3_OUT_OUTPUT,
		CH_4_OUT_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId
	{
		LGT_1_OUT_LIGHT,
		LGT_2_OUT_LIGHT,
		LGT_3_OUT_LIGHT,
		LGT_4_OUT_LIGHT,
		LIGHTS_LEN
	};

	// Some class-wide parameters
	float volt_In[4], volt_Temp;						 // Input voltage
	int splitOrder = 0;									 // Split order menu entry
	int old_num_Channels = 0, num_Channels, cur_Channel; // Number of active channels

	Splitter()
	{
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configInput(POLY_IN_INPUT, "Polyphonic");
		configOutput(CH_1_OUT_OUTPUT, "Channel 1");
		configOutput(CH_2_OUT_OUTPUT, "Channel 2");
		configOutput(CH_3_OUT_OUTPUT, "Channel 3");
		configOutput(CH_4_OUT_OUTPUT, "Channel 4");
	}

	void process(const ProcessArgs &args) override
	{
		// Get the number of channels from the Input, if any
		num_Channels = inputs[POLY_IN_INPUT].getChannels();

		// Did the number of channels change? If so, make sure all lights are out before switching them on as per the new channel count
		// This to avoid flickering lights for no reason, every time process() is called. Also volt array is set to zero to avoid old voltages to be sent
		if (num_Channels != old_num_Channels)
		{
			lights[LGT_1_OUT_LIGHT].setBrightness(0.0);
			lights[LGT_2_OUT_LIGHT].setBrightness(0.0);
			lights[LGT_3_OUT_LIGHT].setBrightness(0.0);
			lights[LGT_4_OUT_LIGHT].setBrightness(0.0);
			volt_In[0] = volt_In[1] = volt_In[2] = volt_In[3] = 0.0f;
			old_num_Channels = num_Channels;
		}

		// Input not connected? Do nothing...
		if (num_Channels == 0)
			return;

		// Sort by Channel Number? Then simply go through the channels in channel order
		// Only one Channel connected? Just copy the single channel, no need to sort
		if (splitOrder == 0 || num_Channels == 1)
		{
			for (cur_Channel = 0; cur_Channel < num_Channels; cur_Channel++)
			{
				volt_In[cur_Channel] = inputs[POLY_IN_INPUT].getVoltage(cur_Channel);
				outputs[cur_Channel].setVoltage(volt_In[cur_Channel]);
				lights[cur_Channel].setBrightness(1.0);
			}
			// Done, so return
			return;
		}

		// Sort by Pitch up? Then get all voltages, sort them ascending and then output
		if (splitOrder == 1)
		{
			// Optimized code for sorting based on # of channels
			if (num_Channels == 2)
			{
				// Get the input values in the array
				volt_In[0] = inputs[POLY_IN_INPUT].getVoltage(0);
				volt_In[1] = inputs[POLY_IN_INPUT].getVoltage(1);

				// Now sort the two values ascending
				if (volt_In[0] > volt_In[1])
				{
					volt_Temp = volt_In[0];
					volt_In[0] = volt_In[1];
					volt_In[1] = volt_Temp;
				}

				// Output the voltages and set the lights
				outputs[CH_1_OUT_OUTPUT].setVoltage(volt_In[0]);
				lights[LGT_1_OUT_LIGHT].setBrightness(1.0);
				outputs[CH_2_OUT_OUTPUT].setVoltage(volt_In[1]);
				lights[LGT_2_OUT_LIGHT].setBrightness(1.0);
				// Done, so return
				return;
			}

			if (num_Channels == 3)
			{
				// Get the input values in the array
				volt_In[0] = inputs[POLY_IN_INPUT].getVoltage(0);
				volt_In[1] = inputs[POLY_IN_INPUT].getVoltage(1);
				volt_In[2] = inputs[POLY_IN_INPUT].getVoltage(2);

				// Now sort the three values ascending, using a simplified bubble sort
				if (volt_In[0] > volt_In[1])
				{
					volt_Temp = volt_In[0];
					volt_In[0] = volt_In[1];
					volt_In[1] = volt_Temp;
				}
				// Now sort the last two elements. Now the last element is the largest
				if (volt_In[1] > volt_In[2])
				{
					volt_Temp = volt_In[1];
					volt_In[1] = volt_In[2];
					volt_In[2] = volt_Temp;
				}
				// Now sort the first two elements and we're done
				if (volt_In[0] > volt_In[1])
				{
					volt_Temp = volt_In[0];
					volt_In[0] = volt_In[1];
					volt_In[1] = volt_Temp;
				}

				// Output the voltages and set the lights
				outputs[CH_1_OUT_OUTPUT].setVoltage(volt_In[0]);
				lights[LGT_1_OUT_LIGHT].setBrightness(1.0);
				outputs[CH_2_OUT_OUTPUT].setVoltage(volt_In[1]);
				lights[LGT_2_OUT_LIGHT].setBrightness(1.0);
				outputs[CH_3_OUT_OUTPUT].setVoltage(volt_In[2]);
				lights[LGT_3_OUT_LIGHT].setBrightness(1.0);
				// Done, so return
				return;
			}

			// 4 channels remaining as last option. Get the input values in the array
			volt_In[0] = inputs[POLY_IN_INPUT].getVoltage(0);
			volt_In[1] = inputs[POLY_IN_INPUT].getVoltage(1);
			volt_In[2] = inputs[POLY_IN_INPUT].getVoltage(2);
			volt_In[3] = inputs[POLY_IN_INPUT].getVoltage(3);

			// 4 channels, so sort the four values ascending. First sort 0 & 1, then 2 & 3 so we have two sorted mini-arrays of length 2
			if (volt_In[0] > volt_In[1])
			{
				volt_Temp = volt_In[0];
				volt_In[0] = volt_In[1];
				volt_In[1] = volt_Temp;
			}
			if (volt_In[2] > volt_In[3])
			{
				volt_Temp = volt_In[2];
				volt_In[2] = volt_In[3];
				volt_In[3] = volt_Temp;
			}
			// Now find the smallest of 0 & 2 and swap
			if (volt_In[0] > volt_In[2])
			{
				volt_Temp = volt_In[0];
				volt_In[0] = volt_In[2];
				volt_In[2] = volt_Temp;
			}
			// Now find the largest of 1 & 3 and swap

			if (volt_In[1] > volt_In[3])
			{
				volt_Temp = volt_In[1];
				volt_In[1] = volt_In[3];
				volt_In[3] = volt_Temp;
			}
			// Now sort the middle two elements 1 & 2 as they may be out of order yet
			if (volt_In[1] > volt_In[2])
			{
				volt_Temp = volt_In[1];
				volt_In[1] = volt_In[2];
				volt_In[2] = volt_Temp;
			}

			// Output the voltages and set the lights
			outputs[CH_1_OUT_OUTPUT].setVoltage(volt_In[0]);
			lights[LGT_1_OUT_LIGHT].setBrightness(1.0);
			outputs[CH_2_OUT_OUTPUT].setVoltage(volt_In[1]);
			lights[LGT_2_OUT_LIGHT].setBrightness(1.0);
			outputs[CH_3_OUT_OUTPUT].setVoltage(volt_In[2]);
			lights[LGT_3_OUT_LIGHT].setBrightness(1.0);
			outputs[CH_4_OUT_OUTPUT].setVoltage(volt_In[3]);
			lights[LGT_4_OUT_LIGHT].setBrightness(1.0);
			// Done, so return
			return;
		}

		// Splitorder = 2, so sort descending

		// Optimized code for sorting based on # of channels
		if (num_Channels == 2)
		{
			// Get the input values in the array
			volt_In[0] = inputs[POLY_IN_INPUT].getVoltage(0);
			volt_In[1] = inputs[POLY_IN_INPUT].getVoltage(1);

			// Now sort the two values ascending
			if (volt_In[0] < volt_In[1])
			{
				volt_Temp = volt_In[0];
				volt_In[0] = volt_In[1];
				volt_In[1] = volt_Temp;
			}

			// Output the voltages and set the lights
			outputs[CH_1_OUT_OUTPUT].setVoltage(volt_In[0]);
			lights[LGT_1_OUT_LIGHT].setBrightness(1.0);
			outputs[CH_2_OUT_OUTPUT].setVoltage(volt_In[1]);
			lights[LGT_2_OUT_LIGHT].setBrightness(1.0);
			// Done, so return
			return;
		}

		if (num_Channels == 3)
		{
			// Get the input values in the array
			volt_In[0] = inputs[POLY_IN_INPUT].getVoltage(0);
			volt_In[1] = inputs[POLY_IN_INPUT].getVoltage(1);
			volt_In[2] = inputs[POLY_IN_INPUT].getVoltage(2);

			// Now sort the three values ascending, using a simplified bubble sort
			if (volt_In[0] < volt_In[1])
			{
				volt_Temp = volt_In[0];
				volt_In[0] = volt_In[1];
				volt_In[1] = volt_Temp;
			}
			// Now sort the last two elements. Now the last element is the largest
			if (volt_In[1] < volt_In[2])
			{
				volt_Temp = volt_In[1];
				volt_In[1] = volt_In[2];
				volt_In[2] = volt_Temp;
			}
			// Now sort the first two elements and we're done
			{
				volt_Temp = volt_In[0];
				volt_In[0] = volt_In[1];
				volt_In[1] = volt_Temp;
			}

			// Output the voltages and set the lights
			outputs[CH_1_OUT_OUTPUT].setVoltage(volt_In[0]);
			lights[LGT_1_OUT_LIGHT].setBrightness(1.0);
			outputs[CH_2_OUT_OUTPUT].setVoltage(volt_In[1]);
			lights[LGT_2_OUT_LIGHT].setBrightness(1.0);
			outputs[CH_3_OUT_OUTPUT].setVoltage(volt_In[2]);
			lights[LGT_3_OUT_LIGHT].setBrightness(1.0);
			// Done, so return
			return;
		}

		// 4 channels remaining as last option. Get the input values in the array
		volt_In[0] = inputs[POLY_IN_INPUT].getVoltage(0);
		volt_In[1] = inputs[POLY_IN_INPUT].getVoltage(1);
		volt_In[2] = inputs[POLY_IN_INPUT].getVoltage(2);
		volt_In[3] = inputs[POLY_IN_INPUT].getVoltage(3);

		// 4 channels, so sort the four values ascending. First sort 0 & 1, then 2 & 3 so we have two sorted mini-arrays of length 2
		if (volt_In[0] < volt_In[1])
		{
			volt_Temp = volt_In[0];
			volt_In[0] = volt_In[1];
			volt_In[1] = volt_Temp;
		}
		if (volt_In[2] < volt_In[3])
		{
			volt_Temp = volt_In[2];
			volt_In[2] = volt_In[3];
			volt_In[3] = volt_Temp;
		}
		// Now find the smallest of 0 & 2 and swap
		if (volt_In[0] < volt_In[2])
		{
			volt_Temp = volt_In[0];
			volt_In[0] = volt_In[2];
			volt_In[2] = volt_Temp;
		}
		// Now find the largest of 1 & 3 and swap
		if (volt_In[1] < volt_In[3])
		{
			volt_Temp = volt_In[1];
			volt_In[1] = volt_In[3];
			volt_In[3] = volt_Temp;
		}
		// Now sort the middle two elements 1 & 2 as they may be out of order yet
		if (volt_In[1] < volt_In[2])
		{
			volt_Temp = volt_In[1];
			volt_In[1] = volt_In[2];
			volt_In[2] = volt_Temp;
		}

		// Output the voltages and set the lights
		outputs[CH_1_OUT_OUTPUT].setVoltage(volt_In[0]);
		lights[LGT_1_OUT_LIGHT].setBrightness(1.0);
		outputs[CH_2_OUT_OUTPUT].setVoltage(volt_In[1]);
		lights[LGT_2_OUT_LIGHT].setBrightness(1.0);
		outputs[CH_3_OUT_OUTPUT].setVoltage(volt_In[2]);
		lights[LGT_3_OUT_LIGHT].setBrightness(1.0);
		outputs[CH_4_OUT_OUTPUT].setVoltage(volt_In[3]);
		lights[LGT_4_OUT_LIGHT].setBrightness(1.0);
		return;
	}

	json_t *
	dataToJson() override
	{
		json_t *rootJ = json_object();

		json_object_set_new(rootJ, "Sort Outputs", json_integer(splitOrder));

		return rootJ;
	}

	void dataFromJson(json_t *rootJ) override
	{
		json_t *splitOrderJ = json_object_get(rootJ, "Sort Outputs");

		if (splitOrderJ)
			splitOrder = json_integer_value(splitOrderJ);
	}
};

struct SplitterWidget : ModuleWidget
{
	SplitterWidget(Splitter *module)
	{
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Splitter.svg")));

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

	void appendContextMenu(Menu *menu) override
	{
		Splitter *module = getModule<Splitter>();

		menu->addChild(new MenuSeparator);

		menu->addChild(createIndexPtrSubmenuItem("Sort Outputs", {"Channel", "Pitch Up", "Pitch Down"}, &module->splitOrder));
	}
};

Model *modelSplitter = createModel<Splitter, SplitterWidget>("Splitter");