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
	u64 getMaxUniformBlockSize();

	u32 getMaxVertexStorageBlocks();
	u32 getMaxFragmentStorageBlocks();
	u32 getMaxGeometryStorageBlocks();
	u32 getMaxTessControlStorageBlocks();
	u32 getMaxTessEvaluationStorageBlocks();
	u32 getMaxComputeStorageBlocks();
	u32 getMaxCombinedStorageBlocks();

	u32 getMaxShaderStorageBlockBindings();
	u64 getMaxShaderStorageBlockSize();

	u32 getMaxComputeGroupCountX();
	u32 getMaxComputeGroupCountY();
	u32 getMaxComputeGroupCountZ();
	u32 getMaxComputeGroupSizeX();
	u32 getMaxComputeGroupSizeY();
	u32 getMaxComputeGroupSizeZ();

	u32 getMaxComputeGroupInvocations();
	u64 getMaxComputeSharedMemorySize();

	u32 getMaxTextureBufferSize();

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
	Never			= 0x0200, // GL_NEVER
	Less			= 0x0201, // GL_LESS
	LessEqual		= 0x0203, // GL_LEQUAL
	Equal			= 0x0202, // GL_EQUAL
	GreaterEqual	= 0x0206, // GL_GEQUAL
	Greater			= 0x0204, // GL_GREATER
	NotEqual		= 0x0205, // GL_NOTEQUAL
	Always			= 0x0207, // GL_ALWAYS
};

void setDepthFunction(DepthCompare function);

enum class Access {
	Read		= 0x88B8, // GL_READ_ONLY
	Write		= 0x88B9, // GL_WRITE_ONLY
	ReadWrite	= 0x88BA, // GL_READ_WRITE
};


GLE_END