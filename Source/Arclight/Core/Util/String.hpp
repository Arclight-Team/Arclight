/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 String.hpp
 */

#pragma once

#include "Meta/Concepts.hpp"
#include "Common/Types.hpp"

#include <string>



namespace String {

	template<CC::Char C>
	constexpr std::basic_string_view<C> view(const std::basic_string<C>& str, SizeT start = 0, SizeT count = -1) noexcept {

		if(start >= str.size()) {
			return {};
		}

		if(count == -1) {
			count = str.size() - start;
		} else if (start + count >= str.size()) {
			return {};
		}

		return std::basic_string_view<C>(&str[start], count);

	}

	template<class... Chars>
	constexpr std::string fromChars(Chars&&... c) requires ((CC::Convertible<Chars, char>) && ...) {
		return {static_cast<char>(c)...};
	}

	template<CC::Char C>
	constexpr std::basic_string<C> quote(std::basic_string_view<C> path) {

		constexpr C whitespace(' ');
		constexpr C quotation('\"');
		constexpr C backslash('\\');

		if (!path.contains(whitespace)) {
			return std::basic_string<C>(path);
		}

		std::basic_string<C> s(path);
		bool escaped = false;

		for (SizeT i = 0; i < s.size(); i++) {

			C c = s[i];

			if ((c == quotation || c == backslash) && !escaped) {

				escaped = false;
				s.insert(i, std::basic_string<C>(1, backslash));
				i++;

			} else if (c == backslash) {
				escaped = true;
			} else {
				escaped = false;
			}

		}

		return quotation + s + quotation;

	}

	template<CC::Char C>
	constexpr std::basic_string<C> quote(const C* path) {
		return quote<C>(std::basic_string_view<C>(path));
	}

	template<CC::Char C>
	constexpr std::basic_string<C> quote(const std::basic_string<C>& path) {
		return quote<C>(std::basic_string_view<C>(path));
	}

	template<CC::Char C>
	constexpr static std::basic_string<C> unquote(std::basic_string_view<C> path) {

		constexpr C whitespace(' ');
		constexpr C quotation('\"');
		constexpr C backslash('\\');

		if (path.size() < 2 || path[0] != quotation || path[path.size() - 1] != quotation) {
			return std::basic_string<C>(path);
		}

		std::basic_string<C> s(path.substr(1, path.size() - 2));
		bool escaped = false;

		for (SizeT i = 0; i < s.size(); i++) {

			C c = s[i];

			if ((c == quotation || c == backslash) && escaped) {

				escaped = false;
				s.erase(i - 1, 1);

				// Cannot underflow because it can only be escaped after one char at least
				i--;

			} else if (c == backslash) {
				escaped = true;
			} else {
				escaped = false;
			}

		}

		return s;

	}

	template<CC::Char C>
	constexpr std::basic_string<C> unquote(const C* path) {
		return unquote<C>(std::basic_string_view<C>(path));
	}

	template<CC::Char C>
	constexpr std::basic_string<C> unquote(const std::basic_string<C>& path) {
		return unquote<C>(std::basic_string_view<C>(path));
	}

	template<CC::Arithmetic A>
	constexpr std::string toHexString(A a, bool upper = true, bool prefix = false) {

		constexpr char v[2][16] = {
			{ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' },
			{ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' }
		};

		constexpr SizeT Size = sizeof(A);

		std::string s;
		s.reserve(Size * 2 + prefix * 2);

		if (prefix) {
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