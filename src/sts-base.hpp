#pragma once

#include <rack.hpp>

using namespace rack;

// Derived types for STS
static const NVGcolor SCHEME_STS_BLUE = nvgRGB(0x00, 0x00, 0xff);
static const NVGcolor SCHEME_STS_RED = nvgRGB(0xff, 0x00, 0x00);
static const NVGcolor SCHEME_STS_GREEN = nvgRGB(0x00, 0xff, 0x00);
static const NVGcolor SCHEME_STS_YELLOW = nvgRGB(0xff, 0xff, 0x00);

template <typename TBase = GrayModuleLightWidget>
struct TSTSBlueLight : TBase {
	TSTSBlueLight() {
		this->addBaseColor(SCHEME_STS_BLUE);
	}
};
using STSBlueLight = TSTSBlueLight<>;

template <typename TBase = GrayModuleLightWidget>
struct TSTSRedLight : TBase {
	TSTSRedLight() {
		this->addBaseColor(SCHEME_STS_RED);
	}
};
using STSRedLight = TSTSRedLight<>;

template <typename TBase = GrayModuleLightWidget>
struct TSTSGreenLight : TBase {
	TSTSGreenLight() {
		this->addBaseColor(SCHEME_STS_GREEN);
	}
};
using STSGreenLight = TSTSGreenLight<>;

template <typename TBase = GrayModuleLightWidget>
struct TSTSYellowLight : TBase {
	TSTSYellowLight() {
		this->addBaseColor(SCHEME_STS_YELLOW);
	}
};
using STSYellowLight = TSTSYellowLight<>;

