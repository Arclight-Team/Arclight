#pragma once

#include <string>
#include "concepts.h"
#include "bits.h"


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



class HashString {

public:

	using Hash = SystemT;

	constexpr static u64 seed64 = 0xA842B0C912ED90ACULL;


	constexpr HashString() : hashValue(0) {}
	constexpr HashString(const std::string& s) noexcept {
		hash(s);
	}

	constexpr void hash(const std::string& s) noexcept {
		//TODO
	}

	constexpr Hash hashed(const std::string& s) noexcept {
		hash(s);
		return hashValue;
	}

	constexpr Hash getHash() const noexcept {
		return hashValue;
	}

	constexpr operator Hash() const noexcept {
		return hashValue;
	}

private:

	Hash hashValue;

};