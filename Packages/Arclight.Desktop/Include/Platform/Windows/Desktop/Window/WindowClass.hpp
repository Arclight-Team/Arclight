/*
 *	 Copyright (c) 2024 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 WindowClass.hpp
 */

#pragma once

#include <memory>
#include <string>



using WindowClassPtr = std::shared_ptr<class WindowClass>;

class WindowClass {

public:

	static WindowClassPtr create(std::string_view name);
	static WindowClassPtr getDefaultWindowClass();

	WindowClass();
	~WindowClass();

	WindowClass(const WindowClass&) = delete;
	WindowClass& operator=(const WindowClass&) = delete;

	WindowClass& operator=(WindowClass&&) = delete;
	WindowClass(WindowClass&&) = default;

	bool isValid() const;
	const std::string& getName() const;

private:

	static WindowClassPtr defaultClass;

	std::string className;
	bool owning;

};
