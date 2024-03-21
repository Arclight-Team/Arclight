/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 Common.hpp
 */

#pragma once

#include <string>
#include "Common/Types.hpp"



namespace Json {

	// TODO: Eventually replace this with UnicodeString
	using StringType = std::string;
	using StringView = std::string_view;

	using IntegerType = i64;
	using FloatType = double;

}



#include "Concepts.inl"
#include "Type.inl"
#include "Exception.inl"
