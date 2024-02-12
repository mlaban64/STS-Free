#pragma once
#include <rack.hpp>


using namespace rack;

// Declare the Plugin, defined in plugin.cpp
extern Plugin* pluginInstance;

// Declare each Model, defined in each module source file
// extern Model* modelMyModule;
extern Model* modelMultiplier;
extern Model* modelSplitter;
extern Model* modelSine_VCO;
extern Model* modelSaw_VCO;
extern Model* modelTriangle_VCO;

// Some useful constants
#define M_2PI 	6.2831853071758f

// Lookup table for the oscillators
#define STS_NUM_WAVE_SAMPLES	1000

extern float sine_wave_lookup_table[STS_NUM_WAVE_SAMPLES];
