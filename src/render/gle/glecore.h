#pragma once

#include "gc.h"


GLE_BEGIN

enum class Alignment {
	None,
	Align2,
	Align4,
	Align8
};

namespace Core {

	//Initializes the context and returns whether it was successful. Call once after the context has been created.
	bool init();

	void printErrors();

	u32 getMaxTextureSize();
	u32 getMaxArrayTextureLayers();
	u32 getMaxMipmapLevels();
	float getMaxTextureAnisotropy();
	u32 getMaxTextureUnits();

}


void setRowUnpackAlignment(Alignment a);
void setRowPackAlignment(Alignment a);


GLE_END