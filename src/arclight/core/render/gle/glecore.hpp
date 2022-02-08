/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 glecore.hpp
 */

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

	u32 getMaxVertexTextureUnits();
	u32 getMaxFragmentTextureUnits();
	u32 getMaxGeometryTextureUnits();
	u32 getMaxTessControlTextureUnits();
	u32 getMaxTessEvaluationTextureUnits();
	u32 getMaxComputeTextureUnits();
	u32 getMaxCombinedTextureUnits();

	u32 getMaxDepthSamples();
	u32 getMaxColorSamples();
	u32 getMaxIntegerSamples();

	u32 getMaxColorAttachments();
	u32 getMaxDrawBuffers();

	u32 getMaxUniformBlockBindings();

	u32 getMaxVertexStorageBlocks();
	u32 getMaxFragmentStorageBlocks();
	u32 getMaxGeometryStorageBlocks();
	u32 getMaxTessControlStorageBlocks();
	u32 getMaxTessEvaluationStorageBlocks();
	u32 getMaxComputeStorageBlocks();
	u32 getMaxCombinedStorageBlocks();

	u32 getMaxStorageBlockBindings();
	u32 getMaxStorageBlockSize();

	u32 getMaxComputeGroupCountX();
	u32 getMaxComputeGroupCountY();
	u32 getMaxComputeGroupCountZ();
	u32 getMaxComputeGroupSizeX();
	u32 getMaxComputeGroupSizeY();
	u32 getMaxComputeGroupSizeZ();

}


enum class Alignment {
	None,
	Align2,
	Align4,
	Align8
};

void setRowUnpackAlignment(Alignment a);
void setRowPackAlignment(Alignment a);

enum class DepthCompare {
	Never,
	Less,
	LessEqual,
	Equal,
	GreaterEqual,
	Greater,
	NotEqual,
	Always
};

void setDepthFunction(DepthCompare function);


GLE_END