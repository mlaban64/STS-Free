#include <rack.hpp>
#include "sts-base.hpp"
using namespace rack;

// Custom Widgets, borrowed from Fundamental module by Andrew Belt et al

struct DigitalDisplay : Widget
{
	std::string fontPath;
	std::string bgText;
	std::string text;
	float fontSize;
	NVGcolor bgColor = nvgRGB(0x0, 0x0, 0x0);
	NVGcolor fgColor = SCHEME_STS_LIGHT_BLUE;
	Vec textPos;

	void prepareFont(const DrawArgs &args)
	{
		// Get font
		std::shared_ptr<Font> font = APP->window->loadFont(fontPath);
		if (!font)
			return;
		nvgFontFaceId(args.vg, font->handle);
		nvgFontSize(args.vg, fontSize);
		nvgTextLetterSpacing(args.vg, 0.0);
		nvgTextAlign(args.vg, NVG_ALIGN_RIGHT);
	}

	void draw(const DrawArgs &args) override
	{
		// Background
		nvgBeginPath(args.vg);
		nvgRoundedRect(args.vg, 0, 0, box.size.x, box.size.y, 2);
		nvgFillColor(args.vg, nvgRGB(0x0, 0x0, 0x0));
		nvgFill(args.vg);

		prepareFont(args);

		// Background text
		nvgFillColor(args.vg, bgColor);
		nvgText(args.vg, textPos.x, textPos.y, bgText.c_str(), NULL);
	}

	void drawLayer(const DrawArgs &args, int layer) override
	{
		if (layer == 1)
		{
			prepareFont(args);

			// Foreground text
			nvgFillColor(args.vg, fgColor);
			nvgText(args.vg, textPos.x, textPos.y, text.c_str(), NULL);
		}
		Widget::drawLayer(args, layer);
	}
};

struct BPM_Display : DigitalDisplay
{
	BPM_Display()
	{
		fontPath = std::string(asset::plugin(pluginInstance, "res/fonts/DSEG14Modern-Regular.ttf"));
		textPos = Vec(43.0, 20.0);
		bgText = "888";
		fontSize = 16;
	}
};

struct CLK_Div_Display : DigitalDisplay
{
	CLK_Div_Display()
	{
		fontPath = std::string(asset::plugin(pluginInstance, "res/fonts/DSEG14Modern-Regular.ttf"));
		textPos = Vec(53.0, 19.0);
		bgText = "8888";
		fontSize = 14;
	}
};

// array with divider factors to scale against master BPM
const float div_to_factor[73] = {1.f / 96.f, 1.f / 92.f, 1.f / 88.f, 1.f / 84.f, 1.f / 80.f, 1.f / 76.f, 1.f / 72.f, 1.f / 68.f, 1.f / 64.f, 1.f / 60.f,
								 1.f / 56.f, 1.f / 52.f, 1.f / 48.f, 1.f / 44.f, 1.f / 40.f, 1.f / 36.f, 1.f / 32.f, 1.f / 28.f, 1.f / 24.f, 1.f / 20.f,
								 1.f / 18.f, 1.f / 16.f, 1.f / 12.f, 1.f / 10.f, 1.f / 9.f, 1.f / 8.f, 1.f / 7.f, 1.f / 6.f, 1.f / 5.f, 1.f / 4.f, 1.f / 3.5f,
								 1.f / 3.f, 1.f / 2.5f, 1.f / 2.f, 1.f / 1.5f, 1.f / 1.33333f, 1.f, 1.33333f, 1.5f, 2.f, 2.5f, 3.f, 3.5f, 4.f, 5.f, 6.f, 7.f,
								 8.f, 9.f, 10.f, 12.f, 16.f, 18.f, 20.f, 24.f, 28.f, 32.f, 36.f, 40.f, 44.f, 48.f, 52.f, 56.f, 60.f, 64.f, 68.f, 72.f, 76.f,
								 80.f, 84.f, 88.f, 92.f, 96.f};

// array with # of master cycles to pass before a phase reset. Odd integers are doubled to avoid a phase shift of half a cycle
const int div_to_msr_cycles[73] = {96, 92, 88, 84, 80, 76, 72, 68, 64, 60, 56, 52, 48, 44, 40, 36, 32, 28, 24, 20, 18, 16, 12, 10, 9, 8, 7, 6, 5, 4, 7,
								   3, 5, 2, 3, 4, 1, 3, 2, 2, 5, 3, 7, 1, 2, 1, 2, 1, 2, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

// array with divider to text mapping for the UI
const char *div_to_text[73] = {"/ 96 . 0", "/ 92 . 0", "/ 88 . 0", "/ 84 . 0", "/ 80 . 0", "/ 76 . 0", "/ 72 . 0", "/ 68 . 0", "/ 64 . 0", "/ 60 . 0", "/ 56 . 0",
							   "/ 52 . 0", "/ 48 . 0", "/ 44 . 0", "/ 40 . 0", "/ 36 . 0", "/ 32 . 0", "/ 28 . 0", "/ 24 . 0", "/ 20 . 0", "/ 18 . 0", "/ 16 . 0",
							   "/ 12 . 0", "/ 10 . 0", "/ 9 . 0", "/ 8 . 0", "/ 7 . 0", "/ 6 . 0", "/ 5 . 0", "/ 4 . 0", "/ 3 . 5", "/ 3 . 0", "/ 2 . 5", "/ 2 . 0",
							   "/ 1 . 5", "/ 1 . 33", "x 1 . 0", "x 1 . 33", "x 1 . 5", "x 2 . 0", "x 2 . 5", "x 3 . 0", "x 3 . 5", "x 4 . 0", "x 5 . 0", "x 6 . 0",
							   "x 7 . 0", "x 8 . 0", "x 9 . 0", "x 10 . 0", "x 12 . 0", "x 16 . 0", "x 18 . 0", "x 20 . 0", "x 24 . 0", "x 28 . 0", "x 32 . 0", "x 36 . 0",
							   "x 40 . 0", "x 44 . 0", "x 48 . 0", "x 52 . 0", "x 56 . 0", "x 60 . 0", "x 64 . 0", "x 68 . 0", "x 72 . 0", "x 76 . 0", "x 80 . 0",
							   "x 84 . 0", "x 88 . 0", "x 92 . 0", "x 96 . 0"};