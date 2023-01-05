/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 concepts.inl
 */

#include "common/concepts.hpp"
#include "common/typetraits.hpp"



class JsonArray;
class JsonObject;

namespace CC {

	template<class T>
	concept JsonNull = CC::Equal<TT::RemoveCVRef<T>, std::nullptr_t>;

	template<class T>
	concept JsonBoolean = CC::Equal<TT::RemoveCVRef<T>, bool>;

	template<class T>
	concept JsonNumber = CC::Arithmetic<TT::RemoveCVRef<T>> && !JsonBoolean<T>;

	template<class T>
	concept JsonString = CC::Equal<TT::RemoveCVRef<T>, Json::StringType> || CC::Convertible<TT::RemoveCVRef<T>, Json::StringType>;

	template<class T>
	concept JsonArray = CC::Equal<TT::RemoveCVRef<T>, ::JsonArray>;

	template<class T>
	concept JsonObject = CC::Equal<TT::RemoveCVRef<T>, ::JsonObject>;

	template<class T>
	concept JsonType = JsonNull<T> || JsonBoolean<T> || JsonNumber<T> || JsonString<T> || JsonArray<T> || JsonObject<T>;

}
