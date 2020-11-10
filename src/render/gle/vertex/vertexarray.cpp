#include "vertexarray.h"

#include GLE_HEADER


GLE_BEGIN


void VertexArray::create() {

	if (!isCreated()) {
		glGenVertexArrays(1, &id);
	}

}



void VertexArray::bind() {

	gle_assert(isCreated(), "Vertex array hasn't been created yet");

	if (!isBound()) {
		glBindVertexArray(id);
	}

}



void VertexArray::destroy() {

	if (isCreated()) {

		glDeleteVertexArrays(1, &id);
		id = invalidVertexArrayID;

	}

}



void VertexArray::setAttribute(u32 index, u8 elements, AttributeType type, u32 stride, u32 offset) {

	gle_assert(isBound(), "Vertex array %d has not been bound (attempted to set attribute settings)", id);
	gle_assert(index < maxVertexAttributes, "Vertex array attribute %d exceeds the limit of %d", index, maxVertexAttributes - 1);
	gle_assert(elements > 0 && elements < 5, "Invalid element count (%d) for vertex array %d at attribute %d", elements, id, index);

	u32 attrType = getAttributeTypeEnum(type);

	switch (type) {

		case AttributeType::Double:
			glVertexAttribLPointer(index, elements, attrType, stride, reinterpret_cast<const void*>(offset));
			break;

		case AttributeType::Byte:
		case AttributeType::UByte:
		case AttributeType::Short:
		case AttributeType::UShort:
		case AttributeType::Int:
		case AttributeType::UInt:
			glVertexAttribIPointer(index, elements, attrType, stride, reinterpret_cast<const void*>(offset));
			break;

		default:
			glVertexAttribPointer(index, elements, attrType, GL_FALSE, stride, reinterpret_cast<const void*>(offset));
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



bool VertexArray::isCreated() const {
	return id != invalidVertexArrayID;
}



bool VertexArray::isBound() const {
	return id == boundVertexArray;
}



u32 VertexArray::getAttributeTypeEnum(AttributeType type) {

	switch (type) {

		case AttributeType::Byte:
			return GL_BYTE;

		case AttributeType::UByte:
			return GL_UNSIGNED_BYTE;

		case AttributeType::Short:
			return GL_SHORT;

		case AttributeType::UShort:
			return GL_UNSIGNED_SHORT;

		case AttributeType::Int:
			return GL_INT;

		case AttributeType::UInt:
			return GL_UNSIGNED_INT;

		case AttributeType::HalfFloat:
			return GL_HALF_FLOAT;

		case AttributeType::Float:
			return GL_FLOAT;

		case AttributeType::Double:
			return GL_DOUBLE;

		case AttributeType::Fixed:
			return GL_FIXED;

		case AttributeType::Int2_10:
			return GL_INT_2_10_10_10_REV;

		case AttributeType::UInt2_10:
			return GL_UNSIGNED_INT_2_10_10_10_REV;

		default:
			gle_assert(false, "Invalid attribute type 0x%X", type);
			return -1;

	}

}


GLE_END