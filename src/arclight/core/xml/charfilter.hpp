#pragma once

#include "xmlc.hpp"


namespace Xml
{
	
#ifdef XML_CHAR_FILTER_LUT
	template<bool Invert>
#else
#ifdef XML_TEMPLATE_CHAR_TYPE
	template<Char CharType, SizeT Count, bool Invert>
#else
	template<SizeT Count, bool Invert>
#endif
#endif
	class CharFilter;

	namespace
	{
		template<class>
		struct IsCharFilter;

#ifdef XML_CHAR_FILTER_LUT
		template<bool Invert>
		struct IsCharFilter<CharFilter<Invert>>
#else
#ifdef XML_TEMPLATE_CHAR_TYPE
		template<Char CharType, SizeT Count, bool Invert>
		struct IsCharFilter<CharFilter<CharType, Count, Invert>>
#else
		template<SizeT Count, bool Invert>
		struct IsCharFilter<CharFilter<Count, Invert>>
#endif
#endif
		{
			static constexpr bool Value = true;
		};
	}

	template<class T>
	concept FilterType = IsCharFilter<T>::Value;

#ifdef XML_CHAR_FILTER_LUT
	template<bool Invert = false>
#else
#ifdef XML_TEMPLATE_CHAR_TYPE
	template<Char CharType = char, SizeT Count = 1, bool Invert = false>
#else
	template<SizeT Count, bool Invert = false>
#endif
#endif
	class CharFilter
	{
	public:

		constexpr CharFilter() {
#ifdef XML_CHAR_FILTER_LUT
			for (u32 i = 0; i < 256; i++) {
				table[i] = 0;
			}
#endif
		}

#ifdef XML_CHAR_FILTER_LUT
#ifdef XML_TEMPLATE_CHAR_TYPE
		template<Char CharType, SizeT Count>
#else
		template<SizeT Count>
#endif
		constexpr CharFilter(const CharType(&chars)[Count]) : CharFilter() {
			
			for (u32 i = 0; i < Count; i++) {
				table[Byte(chars[i])] = 1;
			}
#else
		constexpr CharFilter(const CharType(&chars)[Count]) {
			
			for (u32 i = 0; i < Count; i++) {
				characters[i] = chars[i];
			}
#endif
		}

#ifdef XML_TEMPLATE_CHAR_TYPE
		template<Char CharType>
#endif
		constexpr bool apply(CharType c) const {
			return find(c) ^ Invert;
		}

#ifdef XML_TEMPLATE_CHAR_TYPE
		template<Char CharType>
#endif
		constexpr bool operator()(CharType c) const {
			return apply(c);
		}

		constexpr auto operator!() const {
#ifdef XML_CHAR_FILTER_LUT
			CharFilter<!Invert> filter;

			for (u32 i = 0; i < 256; i++) {
				filter.table[i] = table[i];
			}

			return std::move(filter);
#else
			return CharFilter<Count, !Invert>(characters);
#endif
		}

		// Merge two filters
#ifdef XML_CHAR_FILTER_LUT
		template<bool InvertB>
		constexpr auto operator|(const CharFilter<InvertB>& other) const {

			CharFilter<Invert> filter;

			for (u32 i = 0; i < 256; i++) {
				filter.table[i] = table[i] | other.table[i];
			}
#else
		template<SizeT CountB, bool InvertB>
#ifdef XML_TEMPLATE_CHAR_TYPE
		constexpr auto operator|(const CharFilter<CharType, CountB, InvertB>& other) const {
#else
		constexpr auto operator|(const CharFilter<CountB, InvertB>& other) const {
#endif

			CharFilter<Count + CountB, Invert> filter;

			for (u32 i = 0; i < Count; i++) {
				filter.characters[i] = characters[i];
			}

			for (u32 i = 0; i < CountB; i++) {
				filter.characters[Count + i] = other.characters[i];
			}
#endif

			return std::move(filter);

		}

		// Merge a single character
#ifdef XML_CHAR_FILTER_LUT
#ifdef XML_TEMPLATE_CHAR_TYPE
		template<Char CharType>
#endif
		constexpr auto operator|(CharType c) const {

			CharFilter<Invert> filter(*this);

			filter.table[Byte(c)] = 1;

#else
		constexpr auto operator|(CharType c) const {

			CharFilter<Count + 1, Invert> filter;

			for (u32 i = 0; i < Count; i++) {
				filter.characters[i] = characters[i];
			}

			filter.characters[Count] = c;
#endif

			return std::move(filter);

		}

	private:

#ifdef XML_CHAR_FILTER_LUT
#ifdef XML_TEMPLATE_CHAR_TYPE
		template<Char CharType>
#endif
		constexpr bool find(CharType c) const {
			return table[Byte(c)];
		}
#else
		constexpr bool find(CharType c) const {
			return std::find(std::begin(characters), std::end(characters), c) != std::end(characters);
		}
#endif

#ifdef XML_CHAR_FILTER_LUT
		template<bool>
		friend class CharFilter;

		Byte table[256];
#else
		template<SizeT, bool>
		friend class CharFilter;

		CharType characters[Count];
#endif

	};

#ifdef XML_CHAR_FILTER_LUT
#ifdef XML_TEMPLATE_CHAR_TYPE
	template<Char CharType, bool Invert = false, SizeT Count = 0>
#else
	template<bool Invert = false, SizeT Count = 0>
#endif
	constexpr static auto createFilter(const CharType(&chars)[Count]) {
		return CharFilter<Invert>(chars);
	}
#else
#ifdef XML_TEMPLATE_CHAR_TYPE
	template<Char CharType, bool Invert = false, SizeT Count = 0>
	constexpr static auto createFilter(const CharType(&chars)[Count]) {
		return CharFilter<CharType, Count, Invert>(chars);
	}
#else
	template<bool Invert = false, SizeT Count = 0>
	constexpr static auto createFilter(const CharType(&chars)[Count]) {
		return CharFilter<Count, Invert>(chars);
	}
#endif
#endif


#ifdef XML_TEMPLATE_CHAR_TYPE
	template<Char CharType = char>
	struct Filters
	{
		static constexpr auto Space			= createFilter<CharType>({ ' ', '\t', '\r', '\n' });
		static constexpr auto Bracket		= createFilter<CharType>({ '<', '>' });
		static constexpr auto Quote			= createFilter<CharType>({ '\'', '"' });
		static constexpr auto Name			= !(Space | Bracket | Quote | '?' | '!' | '=' | '&' | '/');
		static constexpr auto Value			= !Bracket;
		static constexpr auto AttributeName	= Name;
	};
#else
	struct Filters
	{
		static constexpr auto Space			= createFilter({ ' ', '\t', '\r', '\n' });
		static constexpr auto Bracket		= createFilter({ '<', '>' });
		static constexpr auto Quote			= createFilter({ '\'', '"' });
		static constexpr auto Name			= !(Space | Bracket | Quote | '?' | '!' | '=' | '&' | '/');
		static constexpr auto Value			= !Bracket;
		static constexpr auto AttributeName	= Name;
	};
#endif

}
