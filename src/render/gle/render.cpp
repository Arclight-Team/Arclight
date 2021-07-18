#include "render.h"
#include GLE_HEADER



GLE_BEGIN



u32 getPrimitiveTypeEnum(PrimType type) {

	switch (type) {

		case PrimType::Point:
			return GL_POINTS;

		case PrimType::LineStrip:
			return GL_LINE_STRIP;

		case PrimType::LineLoop:
			return GL_LINE_LOOP;

		case PrimType::Line:
			return GL_LINES;

		case PrimType::TriangleStrip:
			return GL_TRIANGLE_STRIP;

		case PrimType::TriangleFan:
			return GL_TRIANGLE_FAN;

		case PrimType::Triangle:
			return GL_TRIANGLES;

		case PrimType::LineAdj:
			return GL_LINES_ADJACENCY;

		case PrimType::LineStripAdj:
			return GL_LINE_STRIP_ADJACENCY;

		case PrimType::TriangleAdj:
			return GL_TRIANGLES_ADJACENCY;

		case PrimType::TriangleStripAdj:
			return GL_TRIANGLE_STRIP_ADJACENCY;

		case PrimType::Patch:
			return GL_PATCHES;

		default:
			gle_force_assert("Invalid primitive type 0x%X", type);
			return -1;

	}

}



u32 getIndexTypeEnum(IndexType type) {

	switch (type) {

		case IndexType::Byte:
			return GL_UNSIGNED_BYTE;

		case IndexType::Short:
			return GL_UNSIGNED_SHORT;

		case IndexType::Int:
			return GL_UNSIGNED_INT;

		default:
			gle_force_assert("Invalid index type 0x%X", type);
			return -1;

	}

}


void setClearColor(float r, float g, float b, float a) {
	glClearColor(r, g, b ,a);
}



void clear(u32 bufferMask) {

	u32 mask = 0;
	
	mask |= bufferMask & Color ? GL_COLOR_BUFFER_BIT : 0;
	mask |= bufferMask & Depth ? GL_DEPTH_BUFFER_BIT : 0;
	mask |= bufferMask & Stencil ? GL_STENCIL_BUFFER_BIT : 0;

	gle_assert(mask, "Clear buffer mask cannot be 0");

	glClear(mask);

}



void render(PrimType type, u32 count, u32 start) {

	u32 primType = getPrimitiveTypeEnum(type);
	glDrawArrays(primType, start, count);

}



void renderIndexed(PrimType type, IndexType idxType, u32 idxCount, u32 start) {

	uintptr_t startPtr = start;
	u32 primType = getPrimitiveTypeEnum(type);
	u32 indexType = getIndexTypeEnum(idxType);
	glDrawElements(primType, idxCount, indexType, reinterpret_cast<const void*>(startPtr));

}



void renderInstanced(PrimType type, u32 instances, u32 count, u32 start) {

	u32 primType = getPrimitiveTypeEnum(type);
	glDrawArraysInstanced(primType, start, count, instances);

}



void renderInstancedIndexed(PrimType type, IndexType idxType, u32 instances, u32 idxCount, u32 start) {

	uintptr_t startPtr = start;
	u32 primType = getPrimitiveTypeEnum(type);
	u32 indexType = getIndexTypeEnum(idxType);
	glDrawElementsInstanced(primType, idxCount, indexType, reinterpret_cast<const void*>(startPtr), instances);

}



GLE_END