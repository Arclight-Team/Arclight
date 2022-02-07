/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 globject.hpp
 */

#pragma once

#include "gc.hpp"


GLE_BEGIN

class GLObject {

public:

	inline ~GLObject() noexcept {

		if (id != invalidID) {
			warn("GL Object %d not destroyed, leaking", id);
		}

	}
	
	GLObject(const GLObject& obj) = delete;
	GLObject& operator=(const GLObject& obj) = delete;

	GLObject(GLObject&& obj) noexcept {
		id = obj.id;
		obj.id = invalidID;
	}

	GLObject& operator=(GLObject&& obj) noexcept {
		id = obj.id;
		obj.id = invalidID;
		return *this;
	}

	constexpr bool isCreated() const {
		return id != invalidID;
	}

	virtual bool create() = 0;
	virtual void destroy() = 0;

protected:

	constexpr GLObject() : id(invalidID) {}

	u32 id;

};

GLE_END