#include "plugin.hpp"


Plugin* pluginInstance;

// An array of values to represent the sine wave, as values in the range [-1.0, 1.0]. This can arguebly be regarded as a wavetable
float sine_wave_lookup_table[STS_NUM_WAVE_SAMPLES];



void init(Plugin* p) {
	pluginInstance = p;

	// Add modules here
	// p->addModel(modelMyModule);
	p->addModel(modelMultiplier);
	p->addModel(modelSplitter);
	p->addModel(modelSine_VCO);
	p->addModel(modelSaw_VCO);
	p->addModel(modelTriangle_VCO);

	// Any other plugin initialization may go here.
	// As an alternative, consider lazy-loading assets and lookup tables when your module is created to reduce startup times of Rack.
	static int i;

	
	// Square = same but only odd 1s: 3rd, 5th &c.
	// Triangle = same as square but ¹/₉ of 3rd, ¹/₂₅ of 5th &c. (⅟harmonic²)

	// Populate the sine wave table
	// Filled with a full sine cycle, multiplied by 5.0 to reflect the default +/- 5V audio output levels
	for (i = 0; i < STS_NUM_WAVE_SAMPLES; i++)
	{
		sine_wave_lookup_table[i] = 5.0f * std::sin(M_2PI * ((float) i / STS_NUM_WAVE_SAMPLES));
	}

}

