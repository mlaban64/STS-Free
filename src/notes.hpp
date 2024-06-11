#pragma once

#include <rack.hpp>

using namespace rack;

// Some constants
const float SEMI_TONE = 1 / 12.f;

// Standard chromatic 12 notes from C4 to B4
const float C4_SCALE[12] = {0.f, SEMI_TONE, 2 * SEMI_TONE, 3 * SEMI_TONE,
							4 * SEMI_TONE, 5 * SEMI_TONE, 6 * SEMI_TONE,
							7 * SEMI_TONE, 8 * SEMI_TONE, 9 * SEMI_TONE,
							10 * SEMI_TONE, 11 * SEMI_TONE};

// Contains various notes expressed as voltages

#define C4_V = 0.f
