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


GLE_END