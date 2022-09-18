/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 common.hpp
 */

#pragma once

#include <string>
#include "types.hpp"



namespace Json {

	// TODO: Eventually replace this with UnicodeString
	using StringType = std::string;
	using StringView = std::string_view;

	using IntegerType = i64;
	using FloatType = double;

}



#include "concepts.inl"
#include "type.inl"
#include "exception.inl"
