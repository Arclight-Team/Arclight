/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 vertexarray.hpp
 */

#pragma once

#include "gc.hpp"
#include "globject.hpp"


GLE_BEGIN

enum class AttributeType {
	Byte			= 0x1400, // GL_BYTE
	UByte			= 0x1401, // GL_UNSIGNED_BYTE
	Short			= 0x1402, // GL_SHORT
	UShort			= 0x1403, // GL_UNSIGNED_SHORT
	Int				= 0x1404, // GL_INT
	UInt			= 0x1405, // GL_UNSIGNED_INT
	HalfFloat		= 0x140B, // GL_HALF_FLOAT
	Float			= 0x1406, // GL_FLOAT
	Double			= 0x140A, // GL_DOUBLE
	Fixed			= 0x140C, // GL_FIXED
	Int2u10R		= 0x8D9F, // GL_INT_2_10_10_10_REV
	UInt2u10R		= 0x8368, // GL_UNSIGNED_INT_2_10_10_10_REV
	UInt10f11f11fR	= 0x8C3B, // GL_UNSIGNED_INT_10F_11F_11F_REV
};


enum class AttributeClass {
	Int,
	Float,
	Double
};


class VertexArray : public GLObject {

public:

	//Creates a vertex array if none has been created yet
	virtual bool create() override;

	//Binds the vertex array if not already. Fails if it hasn't been created yet.
	void bind();

	//Destroys a vertex array if it was created once
	virtual void destroy() override;

	//Sets vertex attribute settings
	void setAttribute(u32 index, u8 elements, AttributeType type, SizeT stride, SizeT offset, AttributeClass attrClass = AttributeClass::Float);
	void setDivisor(u32 index, u32 divisor);

	//Enables/disables attributes
	void enableAttribute(u32 index);
	void disableAttribute(u32 index);

	//Checks the given states
	bool isBound() const;

private:

	static inline u32 boundVertexArrayID = invalidBoundID;

};

GLE_END