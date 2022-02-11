/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 render.cpp
 */

#include "render.hpp"
#include GLE_HEADER



GLE_BEGIN



void enableDepthTests() {
	glEnable(GL_DEPTH_TEST);
}



void disableDepthTests() {
	glDisable(GL_DEPTH_TEST);
}



void enableCulling() {
	glEnable(GL_CULL_FACE);
}



void disableCulling() {
	glDisable(GL_CULL_FACE);
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

	u32 primType = static_cast<u32>(type);
	glDrawArrays(primType, start, count);

}



void renderIndexed(PrimType type, IndexType idxType, u32 idxCount, u32 start) {

	uintptr_t startPtr = start;
	u32 primType = static_cast<u32>(type);
	u32 indexType = static_cast<u32>(idxType);
	glDrawElements(primType, idxCount, indexType, reinterpret_cast<const void*>(startPtr));

}



void renderInstanced(PrimType type, u32 instances, u32 count, u32 start) {

	u32 primType = static_cast<u32>(type);
	glDrawArraysInstanced(primType, start, count, instances);

}



void renderInstancedIndexed(PrimType type, IndexType idxType, u32 instances, u32 idxCount, u32 start) {

	uintptr_t startPtr = start;
	u32 primType = static_cast<u32>(type);
	u32 indexType = static_cast<u32>(idxType);
	glDrawElementsInstanced(primType, idxCount, indexType, reinterpret_cast<const void*>(startPtr), instances);

}



void dispatchCompute(u32 sizeX, u32 sizeY, u32 sizeZ) {

	gle_assert(sizeX && sizeY && sizeZ, "Compute work group size cannot be 0");

	glDispatchCompute(sizeX, sizeY, sizeZ);

}



GLE_END