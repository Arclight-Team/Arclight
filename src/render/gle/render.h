#pragma once

#include "gc.h"


GLE_BEGIN


enum class PrimType {
	Point,
	LineStrip,
	LineLoop,
	Line,
	TriangleStrip,
	TriangleFan,
	Triangle,
	LineAdj,
	LineStripAdj,
	TriangleAdj,
	TriangleStripAdj,
	Patch
};


enum class IndexType {
	Byte,
	Short,
	Int
};


enum ClearBuffer {
	Color = 0x1,
	Depth = 0x2,
	Stencil = 0x4
};


u32 getPrimitiveTypeEnum(PrimType type);
u32 getIndexTypeEnum(IndexType type);

//Clearing
void setClearColor(float r, float g, float b, float a);
void clear(u32 bufferMask);

//Standard rendering
void render(PrimType type, u32 count, u32 start = 0);
void renderIndexed(PrimType type, IndexType idxType, u32 idxCount, u32 start = 0);

//Instanced rendering
void renderInstanced(PrimType type, u32 instances, u32 count, u32 start = 0);
void renderInstancedIndexed(PrimType type, IndexType idxType, u32 instances, u32 idxCount, u32 start = 0);

GLE_END