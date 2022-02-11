/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 render.hpp
 */

#pragma once

#include "gc.hpp"


GLE_BEGIN


enum class PrimType {
	Point				= 0x0000, // GL_POINTS
	LineStrip			= 0x0003, // GL_LINE_STRIP
	LineLoop			= 0x0002, // GL_LINE_LOOP
	Line				= 0x0001, // GL_LINES
	TriangleStrip		= 0x0005, // GL_TRIANGLE_STRIP
	TriangleFan			= 0x0006, // GL_TRIANGLE_FAN
	Triangle			= 0x0004, // GL_TRIANGLES
	LineAdj				= 0x000A, // GL_LINES_ADJACENCY
	LineStripAdj		= 0x000B, // GL_LINE_STRIP_ADJACENCY
	TriangleAdj			= 0x000C, // GL_TRIANGLES_ADJACENCY
	TriangleStripAdj	= 0x000D, // GL_TRIANGLE_STRIP_ADJACENCY
	Patch				= 0x000E, // GL_PATCHES
};


enum class IndexType {
	Byte	= 0x1401, // GL_UNSIGNED_BYTE
	Short	= 0x1403, // GL_UNSIGNED_SHORT
	Int		= 0x1405, // GL_UNSIGNED_INT
};


enum ClearBuffer {
	Color = 0x1,
	Depth = 0x2,
	Stencil = 0x4
};


//Depth testing
void enableDepthTests();
void disableDepthTests();

//Culling
void enableCulling();
void disableCulling();

//Clearing
void setClearColor(float r, float g, float b, float a);
void clear(u32 bufferMask);

//Standard rendering
void render(PrimType type, u32 count, u32 start = 0);
void renderIndexed(PrimType type, IndexType idxType, u32 idxCount, u32 start = 0);

//Instanced rendering
void renderInstanced(PrimType type, u32 instances, u32 count, u32 start = 0);
void renderInstancedIndexed(PrimType type, IndexType idxType, u32 instances, u32 idxCount, u32 start = 0);

//Computing
void dispatchCompute(u32 sizeX, u32 sizeY, u32 sizeZ);

GLE_END