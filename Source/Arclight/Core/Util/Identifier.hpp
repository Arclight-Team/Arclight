/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 Identifier.hpp
 */

#pragma once

#include "Bits.hpp"
#include "Meta/Concepts.hpp"
#include "Crypto/Hash/SHA2.hpp"

#include <span>
#include <string>
#include <vector>



template<CC::UnsignedType T>
class Identifier {

public:

	using Type = T;

	constexpr Identifier() noexcept = default;

	constexpr Identifier(Type id) noexcept : id(id) {}

	template<SizeT N>
	consteval Identifier(char(&name)[N]) noexcept {
		assign(name);
	}

	template<class U> requires CC::Equal<U, const char*>
	consteval Identifier(U name) noexcept {
		assign(name);
	}

	consteval Identifier(const std::string& name) noexcept {
		assign(name);
	}


	consteval Identifier& operator=(const std::string& name) noexcept {

		assign(name);
		return *this;

	}

	constexpr Identifier& operator=(Type id) noexcept {

		this->id = id;
		return *this;

	}

	constexpr Type getID() const noexcept {
		return id;
	}

	constexpr operator Type() const noexcept {
		return id;
	}
	
	constexpr bool operator==(const Identifier& other) const noexcept = default;
	constexpr auto operator<=>(const Identifier& other) const noexcept = default;

private:

	consteval void assign(const std::string& name) noexcept {

		std::vector<u8> storage(name.size());

		for (SizeT i = 0; i < name.size(); i++) {
			storage[i] = Bits::cast<u8>(name[i]);
		}

		id = SHA2::hash512t256(storage).toInteger<Type>();

	}

	Type id;

};


using Id32 = Identifier<u32>;
using Id64 = Identifier<u64>;