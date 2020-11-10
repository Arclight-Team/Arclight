#pragma once

#include "../gc.h"


GLE_BEGIN

class VertexArray {

public:

	constexpr VertexArray() : id(invalidVertexArrayID) {}

	//Creates a vertex array if none has been created yet
	void create();

	//Binds the vertex array if not already. Fails if it hasn't been created yet.
	void bind();

	//Destroys a vertex array if it was created once
	void destroy();

	//void setAttribute(u32 index);
	//void setDivisor(u32 index, u32 divisor);

	//Enables/disables attributes
	void enableAttribute(u32 index);
	void disableAttribute(u32 index);

	//Checks the given states
	bool isCreated() const;
	bool isBound() const;

private:

	u32 id;		//ID of the vertex array

	constexpr static inline u32 invalidVertexArrayID = -1;
	constexpr static inline u32 invalidBoundID = -2;

	static inline u32 boundVertexArray = invalidBoundID;

};

GLE_END