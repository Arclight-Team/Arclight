/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 unsupportedoperationexception.hpp
 */

#pragma once

#include "string.hpp"

#include <stdexcept>



class UnsupportedOperationException : public std::runtime_error {

public:

	UnsupportedOperationException() noexcept : UnsupportedOperationException("Operation not supported") {}
	explicit UnsupportedOperationException(const std::string& msg) noexcept : std::runtime_error(msg) {}

	template<class... Args>
	explicit UnsupportedOperationException(const std::string& msg, Args&&... args) noexcept : UnsupportedOperationException(String::format(msg, std::forward<Args>(args)...)) {}

};