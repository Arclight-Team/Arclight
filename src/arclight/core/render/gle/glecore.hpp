#pragma once

#include "gc.hpp"


GLE_BEGIN


namespace Core {

	//Initializes the context and returns whether it was successful. Call once after the context has been created.
	bool init();

	//Prints errors if one (or more) occured
	void printErrors();

}


namespace Limits {

	u32 getMaxTextureSize();
	u32 getMax3DTextureSize();
	u32 getMaxArrayTextureLayers();
	u32 getMaxMipmapLevels();
	float getMaxTextureAnisotropy();
	u32 getMaxTextureUnits();

	u32 getMaxDepthSamples();
	u32 getMaxColorSamples();
	u32 getMaxIntegerSamples();

	u32 getMaxColorAttachments();
	u32 getMaxDrawBuffers();

	u32 getMaxUniformBlockBindings();

}


enum class Alignment {
	None,
	Align2,
	Align4,
	Align8
};

void setRowUnpackAlignment(Alignment a);
void setRowPackAlignment(Alignment a);


GLE_END