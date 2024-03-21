/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 SpringObject.hpp
 */

#pragma once

#include "Common/Types.hpp"



class SpringObject {

public:

	constexpr SpringObject() noexcept = default;

	constexpr u8 getFlags() const noexcept { return flags; }
	constexpr void setFlags(u8 flags) noexcept { this->flags = flags; }
	constexpr void clearFlags() noexcept { flags = 0; }

private:

	u8 flags;

};