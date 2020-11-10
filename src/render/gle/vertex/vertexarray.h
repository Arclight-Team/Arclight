#pragma once

#include "../gc.h"


GLE_BEGIN

enum class AttributeType {
	Byte,
	UByte,
	Short,
	UShort,
	Int,
	UInt,
	HalfFloat,
	Float,
	Double,
	Fixed,
	Int2_10,
	UInt2_10
};


class VertexArray {

public:

	constexpr VertexArray() : id(invalidVertexArrayID) {}

	//Creates a vertex array if none has been created yet
	void create();

	//Binds the vertex array if not already. Fails if it hasn't been created yet.
	void bind();

	//Destroys a vertex array if it was created once
	void destroy();

	//Sets vertex attribute settings
	void setAttribute(u32 index, u8 elements, AttributeType type, u32 stride, u32 offset);
	void setDivisor(u32 index, u32 divisor);

	//Enables/disables attributes
	void enableAttribute(u32 index);
	void disableAttribute(u32 index);

	//Checks the given states
	bool isCreated() const;
	bool isBound() const;

private:

	static u32 getAttributeTypeEnum(AttributeType type);

	u32 id;		//ID of the vertex array

	constexpr static inline u32 invalidVertexArrayID = -1;
	constexpr static inline u32 invalidBoundID = -2;

	static inline u32 boundVertexArray = invalidBoundID;

};

GLE_END