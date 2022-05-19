/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 chariterator.hpp
 */

#pragma once

#include "xmlc.hpp"


namespace Xml
{
	
#ifdef XML_TEMPLATE_CHAR_TYPE
	template<CC::Char CharType = char>
#endif
	class CharIterator
	{
	private:

		using String		= std::basic_string<CharType>;
		using StringView	= std::basic_string_view<CharType>;
		using CharTraits	= std::char_traits<CharType>;

#ifdef XML_TEMPLATE_CHAR_TYPE
		using CharIteratorT = CharIterator<CharType>;
#else
		using CharIteratorT = CharIterator;
#endif

	public:

		constexpr CharIterator() : 
			beg(0), 
			end(0), 
			cur(0)
		{}

		constexpr CharIterator(const CharType* c) :
			beg(c),
			end(c + CharTraits::length(c)),
			cur(beg)
		{}

		constexpr CharIterator(const StringView& sv) :
			beg(sv.data()), 
			end(sv.data() + sv.size()),
			cur(beg)
		{}

		constexpr CharIterator(const CharIterator&) = default;

		constexpr CharIterator& operator=(const CharIterator& other) {

			if (beg != other.beg)
				return *this;

			if (end != other.end)
				return *this;

			if (!other.valid())
				return *this;

			cur = other.cur;

			return *this;

		};


		constexpr SizeT size() const {
			return end - beg;
		}

		constexpr SizeT remaining() const {
			return end - cur;
		}

		constexpr bool valid() const {
			return remaining() && *cur;
		}

		constexpr bool terminated() const {
			return *cur == CharType(0);
		}

		constexpr bool overflow() const {
			return cur > end;
		}



		constexpr bool skip(CharType c, bool invert = false) {

			while (valid() && (*cur != c) ^ invert)
				cur++;

			return valid();

		}

		constexpr bool skip(const FilterType auto& filter) {

			while (valid() && filter(*cur))
				cur++;

			return valid();

		}



		template<class Func>
		constexpr bool cmp() const {
			return valid() && Func::cmp(*cur);
		}

		constexpr bool cmp(CharType ch, int offset = 0) const {
			return (valid() && offset < remaining()) && (cur[offset] == ch);
		}

		constexpr bool cmp(const std::initializer_list<CharType>& li, int offset = 0) const {

			if (li.size() + offset >= remaining())
				return false;

			for (u32 i = 0; i < li.size(); i++) {
				if (li.begin()[i] != cur[offset + i])
					return false;
			}
			
			return true;

		}

		constexpr bool cmp(const FilterType auto& filter) {
			return valid() && filter(*cur);
		}


		
		constexpr CharIterator& operator+=(int x) {
			cur += x;
			return *this;
		}

		constexpr CharIterator& operator-=(int x) {
			cur -= x;
			return *this;
		}

		constexpr CharIterator& operator+(int x) {
			cur += x;
			return *this;
		}

		constexpr CharIterator& operator-(int x) {
			cur -= x;
			return *this;
		}

		constexpr CharIterator operator++(int) {
			CharIterator c = *this;
			cur++;
			return c;
		}

		constexpr CharIterator operator--(int) {
			CharIterator c = *this;
			cur--;
			return c;
		}

		constexpr CharIterator& operator++() {
			cur++;
			return *this;
		}

		constexpr CharIterator& operator--() {
			cur--;
			return *this;
		}

		constexpr const CharType* operator&() const {
			return cur;
		}

		constexpr CharType operator*() {
			return *cur;
		}

	private:
		
		const CharType* const beg;
		const CharType* const end;
		const CharType* cur;

	};

}
