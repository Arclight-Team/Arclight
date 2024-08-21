/*
*	 Copyright (c) 2024 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 Configuration.hpp
 */

#pragma once

#include "Common/Types.hpp"
#include "Meta/TypeTraits.hpp"

#include <map>
#include <array>
#include <string>
#include <variant>


class Configuration {

	using InitValueT = std::variant<bool, std::string, u32, u64, i32, i64, float, double>;

public:

	struct Value {

		Value() = default;

		template<class T>
		Value(const T& t) {
			set(t);
		}

		template<class T, class U = TT::RemoveCVRef<T>>
		void set(const T& t) requires (CC::Convertible<U, std::string> || CC::Float<U> || CC::Integral<U> || CC::Equal<U, InitValueT>) {

			if constexpr (CC::Convertible<U, std::string>) {
				storage.emplace<std::string>(t);
			} else if constexpr (CC::Float<U>) {
				storage.emplace<double>(t);
			} else if constexpr (CC::UnsignedIntegral<U>) {
				storage.emplace<u64>(t);
			} else if constexpr (CC::SignedIntegral<U>) {
				storage.emplace<i64>(t);
			} else if constexpr (CC::Equal<U, InitValueT>) {

				if (std::holds_alternative<bool>(t)) { storage.emplace<u64>(std::get<bool>(t)); }
				else if (std::holds_alternative<std::string>(t)) { storage.emplace<std::string>(std::get<std::string>(t)); }
				else if (std::holds_alternative<u32>(t)) { storage.emplace<u64>(std::get<u32>(t)); }
				else if (std::holds_alternative<u64>(t)) { storage.emplace<u64>(std::get<u64>(t)); }
				else if (std::holds_alternative<i32>(t)) { storage.emplace<i64>(std::get<i32>(t)); }
				else if (std::holds_alternative<i64>(t)) { storage.emplace<i64>(std::get<i64>(t)); }
				else if (std::holds_alternative<float>(t)) { storage.emplace<double>(std::get<float>(t)); }
				else if (std::holds_alternative<double>(t)) { storage.emplace<double>(std::get<double>(t)); }

			}

		}

		template<class T>
		T get(const T& defaultValue) const {

			if (std::holds_alternative<i64>(storage)) {
				if constexpr (CC::Convertible<i64, T>) { return std::get<i64>(storage); }
			} else if (std::holds_alternative<u64>(storage)) {
				if constexpr (CC::Convertible<u64, T>) { return std::get<u64>(storage); }
			} else if (std::holds_alternative<double>(storage)) {
				if constexpr (CC::Convertible<double, T>) { return std::get<double>(storage); }
			} else if (std::holds_alternative<std::string>(storage)) {
				if constexpr (CC::Convertible<std::string, T>) { return std::get<std::string>(storage); }
			}

			return defaultValue;

		}

	private:

		std::variant<i64, u64, double, std::string> storage;

	};


	Configuration() { setupAppConfig(); }

	template<class T>
	void set(const std::string& name, const T& value) {
		configs[splitName(name)] = value;
	}

	template<class T>
	T get(const std::string& name, const T& defaultValue = T()) const {

		auto key = splitName(name);

		if (configs.contains(key)) {

			const Value& value = configs.at(key);
			return value.get<T>(defaultValue);

		}

		return defaultValue;

	}

	void setupAppConfig() {

#ifndef LUMINA_APP_CONFIG
		static std::array<std::pair<std::string, InitValueT>, 0> AppConfig;
#else
		static std::array AppConfig = std::to_array<std::pair<std::string, InitValueT>>({LUMINA_APP_CONFIG});
#endif

		for (const auto& [name, value] : AppConfig) {
			configs[splitName(name)] = value;
		}

	}

	void reset() {
		configs.clear();
	}

private:

	static std::pair<std::string, std::string> splitName(const std::string& name) {

		SizeT delimiter = name.find_first_of(':');
		return delimiter == std::string::npos ? std::pair{"", name} : std::pair{name.substr(0, delimiter), name.substr(delimiter + 1)};

	}

	std::map<std::pair<std::string, std::string>, Value> configs;

};
