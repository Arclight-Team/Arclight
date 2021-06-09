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


u32 getPrimitiveTypeEnum(PrimType type);
u32 getIndexTypeEnum(IndexType type);

//Standard rendering
void render(PrimType type, u32 count, u32 start = 0);
void renderIndexed(PrimType type, IndexType idxType, u32 idxCount, u32 start = 0);

//Instanced rendering
void render(PrimType type, u32 instances, u32 count, u32 start = 0);
void renderIndexed(PrimType type, IndexType idxType, u32 instances, u32 idxCount, u32 start = 0);

GLE_END