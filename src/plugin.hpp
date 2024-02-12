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
