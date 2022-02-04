/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 springobject.hpp
 */

#pragma once

#include "types.hpp"



class SpringObject {

public:

	constexpr SpringObject() noexcept = default;

	bool isDirty() const noexcept;
	void setDirty() noexcept;
	void clearDirty() noexcept;

private:

	u8 flags;

};