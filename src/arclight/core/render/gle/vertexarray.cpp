/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 vertexarray.cpp
 */

#include "vertexarray.hpp"

#include GLE_HEADER


GLE_BEGIN


bool VertexArray::create() {

	if (!isCreated()) {

		glGenVertexArrays(1, &id);

		if (!id) {

			id = invalidID;
			return false;

		}

	}

	return true;

}



void VertexArray::bind() {

	gle_assert(isCreated(), "Vertex array hasn't been created yet");

	if (!isBound()) {
		glBindVertexArray(id);
		boundVertexArrayID = id;
	}

}



void VertexArray::destroy() {

	if (isCreated()) {

		if (isBound()) {
			boundVertexArrayID = invalidBoundID;
		}

		glDeleteVertexArrays(1, &id);
		id = invalidID;

	}

}



void VertexArray::setAttribute(u32 index, u8 elements, AttributeType type, SizeT stride, SizeT offset, AttributeClass attrClass) {

	gle_assert(isBound(), "Vertex array %d has not been bound (attempted to set attribute settings)", id);
	gle_assert(index < maxVertexAttributes, "Vertex array attribute %d exceeds the limit of %d", index, maxVertexAttributes - 1);
	gle_assert(elements > 0 && elements < 5, "Invalid element count (%d) for vertex array %d at attribute %d", elements, id, index);

	u32 attrType = static_cast<u32>(type);
	const void* ptrOffset = reinterpret_cast<const void*>(static_cast<uintptr_t>(offset));

	switch (attrClass) {

		case AttributeClass::Double:
			gle_assert(type == AttributeType::Double, "Cannot bind type to attribute class 'double' for vertex array %d", id);
			glVertexAttribLPointer(index, elements, attrType, stride, ptrOffset);
			break;

		case AttributeClass::Int:
			gle_assert(type == AttributeType::Byte || type == AttributeType::Short || type == AttributeType::Int ||
					   type == AttributeType::UByte || type == AttributeType::UShort || type == AttributeType::UInt, 
					   "Cannot bind type to attribute class 'int' for vertex array %d", id);
			glVertexAttribIPointer(index, elements, attrType, stride, ptrOffset);
			break;

		default:
			glVertexAttribPointer(index, elements, attrType, GL_FALSE, stride, ptrOffset);
			break;

	}

}



void VertexArray::setDivisor(u32 index, u32 divisor) {

	gle_assert(isBound(), "Vertex array %d has not been bound (attempted to set attribute divisor)", id);
	gle_assert(index < maxVertexAttributes, "Vertex array attribute %d exceeds the limit of %d", index, maxVertexAttributes - 1);

	glVertexAttribDivisor(index, divisor);

}



void VertexArray::enableAttribute(u32 index) {

	gle_assert(isBound(), "Vertex array %d has not been bound (attempted to enable attributes)", id);
	gle_assert(index < maxVertexAttributes, "Vertex array attribute %d exceeds the limit of %d", index, maxVertexAttributes - 1);

	glEnableVertexAttribArray(index);

}



void VertexArray::disableAttribute(u32 index) {

	gle_assert(isBound(), "Vertex array %d has not been bound (attempted to disable attributes)", id);
	gle_assert(index < maxVertexAttributes, "Vertex array attribute %d exceeds the limit of %d", index, maxVertexAttributes - 1);

	glDisableVertexAttribArray(index);

}



bool VertexArray::isBound() const {
	return id == boundVertexArrayID;
}


GLE_END