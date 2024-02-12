#include "plugin.hpp"


Plugin* pluginInstance;





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
}

