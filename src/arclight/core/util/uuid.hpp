/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 uuid.hpp
 */

#pragma once

#include "concepts.hpp"
#include "crypto/hash/sha2.hpp"

#include <span>
#include <string>
#include <vector>



class UUID {

public:

	constexpr UUID() noexcept = default;

	constexpr UUID(u64 id) noexcept : uuid(id) {}

	template<SizeT N>
	constexpr UUID(char(&arr)[N]) noexcept : UUID(std::string(arr)) {}

	template<class T> requires Equal<T, const char*>
	constexpr UUID(T name) noexcept : UUID(std::string(name)) {}

	constexpr UUID(const std::string& name) noexcept {
		assign(name);
	}


	constexpr UUID& operator=(const std::string& name) noexcept {

		assign(name);
		return *this;

	}

	constexpr UUID& operator=(u64 id) noexcept {

		uuid = id;
		return *this;

	}

	constexpr u64 getID() const noexcept {
		return uuid;
	}

	constexpr operator u64() const noexcept {
		return uuid;
	}

private:

	constexpr void assign(const std::string& name) noexcept {

		if (std::is_constant_evaluated()) {

			std::vector<u8> storage(name.size());

			for (SizeT i = 0; i < name.size(); i++) {
				storage[i] = Bits::cast<u8>(name[i]);
			}

			uuid = SHA2::hash512t256(storage).toInteger<u64>();

		} else {

			uuid = SHA2::hash512t256({Bits::toByteArray(name.data()), name.size()}).toInteger<u64>();

		}

	}

	u64 uuid;

};