#include <rack.hpp>
using namespace rack;

// Custom Widgets, borrowed from Fundamental module by Andrew Belt et al

struct DigitalDisplay : Widget
{
	std::string fontPath;
	std::string bgText;
	std::string text;
	float fontSize;
	NVGcolor bgColor = nvgRGB(0x46, 0x46, 0x46);
	NVGcolor fgColor = SCHEME_BLUE;
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
		nvgFillColor(args.vg, nvgRGB(0x19, 0x19, 0x19));
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
		textPos = Vec(36.0, 20.0);
		bgText = "888";
		fontSize = 16;
	}
};

struct CLK1_Div_Display : DigitalDisplay
{
	CLK1_Div_Display()
	{
		fontPath = std::string(asset::plugin(pluginInstance, "res/fonts/DSEG14Modern-Regular.ttf"));
		textPos = Vec(36.0, 20.0);
		bgText = "XXX888";
		fontSize = 16;
	}
};