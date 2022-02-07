/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 char.hpp
 */

#pragma once


namespace Character {


	constexpr bool isLower(char c) noexcept {
		return c >= 'a' && c <= 'z';
	}

	constexpr bool isUpper(char c) noexcept {
		return c >= 'A' && c <= 'Z';
	}

	constexpr bool isAlpha(char c) noexcept {
		return isLower(c) || isUpper(c);
	}

	constexpr bool isDigit(char c) noexcept {
		return c >= '0' && c <= '9';
	}

	constexpr bool isSpace(char c) noexcept {
		return c == ' ';
	}

	constexpr bool isTab(char c) noexcept {
		return c == '\t';
	}

	constexpr bool isIndent(char c) noexcept {
		return isSpace(c) || isTab(c);
	}

	constexpr bool isControl(char c) noexcept {
		return c < ' ' || c == 0x7F;
	}

	constexpr bool isPunctuation(char c) noexcept {
		return  (c >= '!' && c <= '/') ||
				(c >= ':' && c <= '@') ||
				(c >= '[' && c <= '`') ||
				(c >= '{' && c <= '~');
	}

	constexpr bool isRenderable(char c) noexcept {
		return c >= '!' && c <= '~';
	}

	constexpr bool isHexDigit(char c) noexcept {
		return isDigit(c) || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
	}


}