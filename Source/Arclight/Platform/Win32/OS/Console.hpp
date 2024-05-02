/*
 *	 Copyright (c) 2024 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 Console.hpp
 */

#pragma once

#include "Math/Vector.hpp"
#include "OS/Common.hpp"



namespace OS::Console {

	enum class Flags {
		None = 0x0,
		Insert = 0x1,
		LineInput = 0x2,
		ProcessInput = 0x4,
		Edit = 0x8,
		ProcessOutput = 0x10,
		Wrap = 0x20
	};

	ARC_CREATE_BITMASK_ENUM(Flags)

	void initialize();
	void destroy();

	bool exists();

	std::string getTitle();
	bool isCursorVisible();
	Vec2i getCursorPosition();
	u32 getFontSize();
	std::string getFontName();
	Vec2i getBufferSize();
	Vec2i getWindowSize();
	Flags getFlags();

	void setTitle(const std::string& title);
	void setCursorVisibility(bool visible);
	void setCursorPosition(u32 x, u32 y);
	void setFontSize(u32 size);
	void setFontName(const std::string& name);
	void setBufferSize(u32 width, u32 height);
	void setFlags(Flags flags);

	void clear();
	void fill(char16_t c);

	std::string read(u32 count);

	SizeT write(const std::string& text);
	SizeT writeLine(const std::string& text);
	bool write(const std::string& text, const Vec2i& pos, const Vec2i& size);

	inline OS::Handle stdinHandle;
	inline OS::Handle stdoutHandle;

};
