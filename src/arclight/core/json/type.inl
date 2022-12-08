/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 type.inl
 */



namespace Json {

	enum class Type {

		None,
		String,
		Number,
		Object,
		Array,
		Boolean,
		Null,

	};

	template<CC::JsonValue V>
	constexpr Type typeOf() {

		if constexpr (CC::JsonNull<V>) {
			return Type::Null;
		} else if constexpr (CC::JsonBoolean<V>) {
			return Type::Boolean;
		} else if constexpr (CC::JsonNumber<V>) {
			return Type::Number;
		} else if constexpr (CC::JsonString<V>) {
			return Type::String;
		} else if constexpr (CC::JsonArray<V>) {
			return Type::Array;
		} else if constexpr (CC::JsonObject<V>) {
			return Type::Object;
		}

		// Unreacheable

	}

	template<CC::JsonValue V>
	constexpr Type typeOf(const V& value) {
		return typeOf<V>();
	}

	constexpr const char* typeName(Type type) {
		
		switch (type) {

			case Type::None:
				return "None";

			case Type::String:
				return "String";

			case Type::Number:
				return "Number";

			case Type::Object:
				return "Object";

			case Type::Array:
				return "Array";

			case Type::Boolean:
				return "Boolean";

			case Type::Null:
				return "Null";

		}

		return "None";

	}

}
