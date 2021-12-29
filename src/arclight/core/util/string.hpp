/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 string.hpp
 */

#pragma once

#include "bits.hpp"
#include "concepts.hpp"

#include <string>


namespace String {

	//Noexcept because the string is checked against any faults
	constexpr std::string_view view(const std::string& str, SizeT start = 0, SizeT count = -1) noexcept {

		if(start >= str.size()) {
			return std::string_view();
		}

		if(count == -1) {
			count = str.size() - start;
		} else if (start + count >= str.size()) {
			return std::string_view();
		}

		return std::string_view(&str[start], count);

	}

	template<class... Chars>
	constexpr std::string fromChars(Chars&&... c) requires ((Convertible<Chars, char>) && ...) {
		return {static_cast<char>(c)...};
	}

	template<Arithmetic A>
	constexpr std::string toHexString(A a, bool upper = true, bool prefix = false) {

		constexpr char v[2][16] = {
			{ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' },
			{ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' }
		};

		constexpr SizeT Size = sizeof(A);

		std::string s;
		s.reserve(Size * 2 + prefix * 2);

		if(prefix) {
			s += "0x";
		}

		for(SizeT i = 0; i < Size; i++) {

			u8 byte = (a >> ((Size - i - 1) * 8)) & 0xFF;

			s += v[upper][byte >> 4];
			s += v[upper][byte & 0xF];

		}

		return s;

	}

	template<class... Args>
	std::string format(std::string message, Args&&... args) noexcept {

		try {
			size_t length = std::snprintf(nullptr, 0, message.c_str(), args...);

			std::string out;
			out.resize(length + 1);

			std::snprintf(out.data(), out.size(), message.c_str(), args...);

			out.erase(out.end() - 1);

			return out;
		}
		catch (std::exception&) {
			//We cannot log here, so return an empty string
			return "";
		}

	}

	template<class... Args>
	std::wstring format(std::wstring message, Args&&... args) noexcept {

		try {
			size_t length = std::snprintf(nullptr, 0, message.c_str(), args...);

			std::wstring out;
			out.resize(length + 1);

			std::swprintf(out.data(), out.size(), message.c_str(), args...);

			out.erase(out.end() - 1);

			return out;
		}
		catch (std::exception&) {
			//We cannot log here, so return an empty string
			return L"";
		}

	}

}