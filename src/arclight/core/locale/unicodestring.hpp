/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 unicodestring.hpp
 */

#pragma once

#include "unicode.hpp"
#include "util/concepts.hpp"
#include "util/typetraits.hpp"

#include <stdexcept>
#include <vector>
#include <string>
#include <span>


/*
 *  Helper base class to optimize access of UTF-8/UTF-16 encoded data
 */
template<Unicode::Encoding E>
struct UnicodeStringHelper {

	using DistanceContainer = std::vector<SizeT>;
	constexpr static SizeT CPDistance = Unicode::isUTF8<E>() ? 16 : 32;

	constexpr UnicodeStringHelper() noexcept : totalCodepoints(0) {}
	constexpr UnicodeStringHelper(const DistanceContainer& container, SizeT cps) : distances(container), totalCodepoints(cps) {}
	constexpr UnicodeStringHelper(DistanceContainer&& container, SizeT cps) : distances(std::move(container)), totalCodepoints(cps) {}

	DistanceContainer distances;
	SizeT totalCodepoints;

};

template<>
struct UnicodeStringHelper<Unicode::UTF32> {};



/*
 *  String class capable of holding Unicode codepoints encoded in UTF
 *
 *  Possible improvements:
 *  - C++23 resize_and_overwrite to avoid copy construction of CharT
 *  - Weaken UTFProxy to decay in template contexts
 */
template<Unicode::Encoding E>
class UnicodeString : private UnicodeStringHelper<E> {

	template<class StringView>
	constexpr static bool IsStringViewConvertible() noexcept {

		return (ImpConvertible<const StringView&, std::basic_string_view<char8_t>> && !ImpConvertible<const StringView&, const char8_t*>)
			   || (ImpConvertible<const StringView&, std::basic_string_view<char16_t>> && !ImpConvertible<const StringView&, const char16_t*>)
			   || (ImpConvertible<const StringView&, std::basic_string_view<char32_t>> && !ImpConvertible<const StringView&, const char32_t*>);

	}

	template<class StringView>
	consteval static Unicode::Encoding GetStringViewEncoding() noexcept {

		if constexpr (ImpConvertible<const StringView&, std::basic_string_view<char8_t>> && !ImpConvertible<const StringView&, const char8_t*>) {
			return Unicode::UTF8;
		} else if constexpr (ImpConvertible<const StringView&, std::basic_string_view<char16_t>> && !ImpConvertible<const StringView&, const char16_t*>) {
			return Unicode::UTF16;
		} else if constexpr (ImpConvertible<const StringView&, std::basic_string_view<char32_t>> && !ImpConvertible<const StringView&, const char32_t*>) {
			return Unicode::UTF32;
		} else {
			static_assert("No suitable encoding exists for given type");
		}

	}

public:

	constexpr static Unicode::Encoding EncodingType = E;

	using Traits = typename Unicode::UTFEncodingTraits<E>;
	using CharT = typename Traits::Type;

	using value_type = CharT;
	using pointer = value_type*;
	using reference = value_type&;
	using const_reference = const value_type&;
	using const_pointer = const value_type*;

	using Container = std::basic_string<CharT>;
	using Codepoint = Unicode::Codepoint;
	using Base = UnicodeStringHelper<E>;

	static_assert(std::is_nothrow_copy_constructible_v<CharT> && std::is_nothrow_copy_assignable_v<CharT>, "Type cannot throw on copy");


	template<class StringView>
	constexpr static bool StringViewConvertible = IsStringViewConvertible<StringView>();

	template<class StringView>
	using StringViewType = std::basic_string_view<typename Unicode::UTFEncodingTraits<GetStringViewEncoding<StringView>()>::Type>;


	//Proxy object for element access
	template<bool Const>
	class UTFProxyBase {

	public:

		using IteratorT = typename UnicodeString<E>::const_iterator;
		using PointerT = typename IteratorT::pointer;
		using StringT = TT::ConditionalConst<Const, UnicodeString>;

		constexpr explicit UTFProxyBase(StringT& ustr, const IteratorT& it) noexcept : ustr(ustr), it(it) {}

		constexpr UTFProxyBase& operator=(Codepoint codepoint) requires(!Const) {

			IteratorT nextIt = it;
			ustr.replace(it, ++nextIt, codepoint);

			return *this;

		}

		constexpr PointerT operator&() const noexcept {
			return it.getPointer();
		}

		constexpr operator Codepoint() const noexcept {
			return Unicode::decode<E>(it.getPointer());
		}

	private:

		StringT& ustr;
		IteratorT it;

	};

	using UTFProxy = UTFProxyBase<false>;
	using UTFImmutableProxy = UTFProxyBase<true>;

	//Bidirectional iterator for UTF-8/16
	//Contiguous iterator for UTF-32
	template<bool Const>
	class Iterator {

	public:

		using iterator_category = TT::Conditional<Unicode::isUTF32<E>(), std::contiguous_iterator_tag, std::bidirectional_iterator_tag>;
		using difference_type = std::ptrdiff_t;
		using value_type = TT::ConditionalConst<Const, typename UnicodeString<E>::value_type>;
		using pointer = value_type*;
		using reference = value_type&;
		using element_type = value_type;

		constexpr Iterator() noexcept : cpIdx(0), sp(nullptr) {}
		constexpr Iterator(SizeT cpIndex, pointer p) noexcept : cpIdx(cpIndex), sp(p) {}
		constexpr Iterator(const Iterator<true>& it) noexcept requires(Const) : Iterator(it.cpIdx, it.sp) {}
		constexpr Iterator(const Iterator<true>& it) noexcept requires (!Const) = delete;

		constexpr reference operator*() const noexcept { return *sp; }
		constexpr pointer operator->() const noexcept { return sp; }

		constexpr Iterator& operator++() noexcept {
			advance();
			return *this;
		}

		constexpr Iterator& operator--() noexcept {
			retreat();
			return *this;
		}

		constexpr Iterator operator++(int) noexcept {
			auto cpy = *this;
			++(*this);
			return cpy;
		}

		constexpr Iterator operator--(int) noexcept {
			auto cpy = *this;
			--(*this);
			return cpy;
		}

		template<bool ConstOther>
		constexpr bool operator==(const Iterator<ConstOther>& other) const noexcept {
			return sp == other.sp;
		}

		template<bool ConstOther>
		constexpr auto operator<=>(const Iterator<ConstOther>& other) const noexcept {
			return sp <=> other.sp;
		}

		constexpr Codepoint getCodepoint() const noexcept {
			return Unicode::decode<E>(sp);
		}

		constexpr SizeT getCodepointIndex() const noexcept {
			return cpIdx;
		}

		constexpr pointer getPointer() const noexcept { return sp; }

		//Contiguous extension for UTF-32
		constexpr Iterator operator+(SizeT n) const noexcept requires(Unicode::isUTF32<E>())    { return Iterator(cpIdx + n, sp + n); }
		constexpr Iterator operator-(SizeT n) const noexcept requires(Unicode::isUTF32<E>())    { return Iterator(cpIdx - n, sp - n); }
		constexpr Iterator& operator+=(SizeT n) noexcept requires(Unicode::isUTF32<E>())        { sp += n; cpIdx += n; return *this; }
		constexpr Iterator& operator-=(SizeT n) noexcept requires(Unicode::isUTF32<E>())        { sp -= n; cpIdx -= n; return *this; }
		constexpr reference operator[](SizeT n) const noexcept requires(Unicode::isUTF32<E>())  { return *(*this + n); }

		template<bool ConstOther>
		constexpr difference_type operator-(const Iterator<ConstOther>& other) const noexcept requires(Unicode::isUTF32<E>()) { return sp - other.sp; }

		friend constexpr Iterator operator+(SizeT n, const Iterator& it) noexcept requires(Unicode::isUTF32<E>()) { return it + n; }

	private:

		template<bool ConstOther>
		friend class Iterator;

		constexpr void advance() noexcept {
			sp += Unicode::getEncodedSize<E>(sp);
			cpIdx++;
		}

		constexpr void retreat() noexcept {
			sp -= Unicode::getEncodedSizeBackwards<E>(sp);
			cpIdx--;
		}

		SizeT cpIdx;
		pointer sp;

	};


	using iterator = Iterator<false>;
	using const_iterator = Iterator<true>;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;


	constexpr UnicodeString() noexcept = default;

	constexpr UnicodeString(SizeT count, Codepoint cp) {
		assign(count, cp);
	}

	template<Unicode::Encoding Enc>
	constexpr UnicodeString(const UnicodeString<Enc>& ustr, SizeT ssIndex) {
		assign(ustr, ssIndex);
	}

	template<Unicode::Encoding Enc>
	constexpr UnicodeString(const UnicodeString<Enc>& ustr, SizeT ssIndex, SizeT ssSize) {
		assign(ustr, ssIndex, ssSize);
	}

	template<Char C>
	constexpr UnicodeString(const C* s, SizeT count) {
		assign(s, count);
	}

	template<Char C>
	constexpr UnicodeString(const C* s) {
		assign(s);
	}

	template<std::input_iterator InputIt>
	constexpr UnicodeString(const InputIt& first, const InputIt& last) {
		assign(first, last);
	}

	template<Char C>
	constexpr UnicodeString(const std::initializer_list<C>& list) {
		assign(list);
	}

	template<class StringView>
	constexpr explicit UnicodeString(const StringView& sv) requires StringViewConvertible<StringView> {
		assign(sv);
	}

	template<class StringView>
	constexpr explicit UnicodeString(const StringView& sv, SizeT sIndex, SizeT sCount = SizeT(-1)) requires StringViewConvertible<StringView> {
		assign(sv, sIndex, sCount);
	}


	template<Unicode::Encoding Enc>
	constexpr UnicodeString(const UnicodeString<Enc>& ustr) {
		assign(ustr);
	}

	template<Unicode::Encoding Enc>
	constexpr UnicodeString(UnicodeString<Enc>&& ustr) noexcept requires(E == Enc && Unicode::isUTF32<E>()) : str(std::move(ustr.str)) {}

	template<Unicode::Encoding Enc>
	constexpr UnicodeString(UnicodeString<Enc>&& ustr) noexcept requires(E == Enc && !Unicode::isUTF32<E>()) : Base(std::move(ustr.distances), ustr.totalCodepoints), str(std::move(ustr.str)) {}

	template<Unicode::Encoding Enc>
	constexpr UnicodeString(UnicodeString<Enc>&& ustr) requires(E != Enc) {
		assign(std::move(ustr));
	}

	constexpr UnicodeString(std::nullptr_t) = delete;

	constexpr ~UnicodeString() noexcept = default;


	template<Unicode::Encoding Enc>
	constexpr UnicodeString& operator=(const UnicodeString<Enc>& ustr) {
		return assign(ustr);
	}

	template<Unicode::Encoding Enc>
	constexpr UnicodeString& operator=(UnicodeString<Enc>&& ustr) noexcept(noexcept(assign(std::move(ustr)))) {
		return assign(std::move(ustr));
	}

	template<Char C>
	constexpr UnicodeString& operator=(const C* s) {
		return assign(s);
	}

	constexpr UnicodeString& operator=(Codepoint codepoint) {
		return assign(codepoint);
	}

	template<Char C>
	constexpr UnicodeString& operator=(const std::initializer_list<C>& list) {
		return assign(list);
	}

	template<class StringView>
	constexpr UnicodeString& operator=(const StringView& sv) requires StringViewConvertible<StringView> {
		return assign(sv);
	}

	constexpr UnicodeString& operator=(std::nullptr_t) = delete;


	constexpr UnicodeString& assign(Codepoint codepoint) {
		return assign(1, codepoint);
	}

	constexpr UnicodeString& assign(SizeT count, Codepoint codepoint) {

		assignCodepoint(codepoint, count);
		return *this;

	}

	template<Unicode::Encoding Enc>
	constexpr UnicodeString& assign(const UnicodeString<Enc>& ustr) {

		if constexpr (E == Enc) {
			copyDirect(ustr);
		} else {
			assign(ustr.begin(), ustr.end());
		}

		return *this;

	}

	template<Unicode::Encoding Enc>
	constexpr UnicodeString& assign(const UnicodeString<Enc>& ustr, SizeT ssIndex, SizeT ssSize = SizeT(-1)) {

		ssSize = clampSubsize(ssIndex, ssSize, ustr.size());

		SizeT startOffset = ustr.getInclusiveOffsetOrThrow(ssIndex);
		SizeT endOffset = ustr.getInclusiveOffsetOrThrow(ssIndex + ssSize);

		return assign(ustr.str.data() + startOffset, ustr.str.data() + endOffset);

	}

	template<Unicode::Encoding Enc>
	constexpr UnicodeString& assign(UnicodeString<Enc>&& ustr) noexcept(E == Enc && noexcept(moveDirect(std::move(ustr)))) {

		if constexpr (E == Enc) {
			moveDirect(std::move(ustr));
		} else {
			assign(ustr.begin(), ustr.end());
		}

		return *this;

	}

	template<Char C>
	constexpr UnicodeString& assign(const C* s, SizeT count) {

		std::span<const C> span {s, s + count};
		assignByRange(span.begin(), span.end());

		return *this;

	}

	template<Char C>
	constexpr UnicodeString& assign(const C* s) {
		return assign(s, arraySize(s));
	}

	template<std::input_iterator InputIt>
	constexpr UnicodeString& assign(const InputIt& first, const InputIt& last) {
		return assignByRange(first, last);
	}

	template<Char C>
	constexpr UnicodeString& assign(const std::initializer_list<C>& list) {
		return assign(list.begin(), list.end());
	}

	template<class StringView>
	constexpr UnicodeString& assign(const StringView& sv) requires StringViewConvertible<StringView> {

		StringViewType<StringView> view = sv;
		return assign(view.begin(), view.end());

	}

	template<class StringView>
	constexpr UnicodeString& assign(const StringView& sv, SizeT sIndex, SizeT sCount = SizeT(-1)) requires StringViewConvertible<StringView> {

		StringViewType<StringView> view = sv;
		view = view.substr(sIndex, sCount);

		return assign(view.begin(), view.end());

	}


	constexpr auto getAllocator() const noexcept {
		return str.get_allocator();
	}


	constexpr UTFProxy at(SizeT index) {
		return UTFProxy(*this, const_iterator(index, str.data() + getOffsetOrThrow(index)));
	}

	constexpr UTFImmutableProxy at(SizeT index) const {
		return UTFImmutableProxy(*this, const_iterator(index, str.data() + getOffsetOrThrow(index)));
	}

	constexpr UTFProxy operator[](SizeT index) noexcept(noexcept(std::declval<Container>().operator[](0))) {
		return UTFProxy(*this, const_iterator(index, str.data() + getOffsetDirect(index)));
	}

	constexpr UTFImmutableProxy operator[](SizeT index) const noexcept(noexcept(std::declval<Container>().operator[](0))) {
		return UTFImmutableProxy(*this, const_iterator(index, str.data() + getOffsetDirect(index)));
	}

	constexpr UTFProxy front() noexcept {
		return UTFProxy(*this, const_iterator(0, str.data()));
	}

	constexpr UTFImmutableProxy front() const noexcept {
		return UTFImmutableProxy(*this, const_iterator(0, str.data()));
	}

	constexpr UTFProxy back() noexcept {
		return UTFProxy(*this, const_iterator(size() - 1, const_cast<CharT*>(fastBack())));
	}

	constexpr UTFImmutableProxy back() const noexcept {
		return UTFImmutableProxy(*this, const_iterator(size() - 1, fastBack()));
	}

	constexpr CharT* data() noexcept {
		return str.data();
	}

	constexpr const CharT* data() const noexcept {
		return str.data();
	}

	constexpr const CharT* cStr() const noexcept {
		return str.c_str();
	}

	constexpr operator std::basic_string_view<CharT>() const noexcept {
		return std::basic_string_view<CharT>(str.data(), str.size());
	}


	constexpr iterator begin() noexcept {
		return iterator(0, str.data());
	}

	constexpr const_iterator begin() const noexcept {
		return const_iterator(0, str.data());
	}

	constexpr const_iterator cbegin() const noexcept {
		return const_iterator(0, str.data());
	}

	constexpr reverse_iterator rbegin() noexcept {
		return reverse_iterator(end());
	}

	constexpr const_reverse_iterator rbegin() const noexcept {
		return const_reverse_iterator(end());
	}

	constexpr const_reverse_iterator crbegin() const noexcept {
		return const_reverse_iterator(cend());
	}

	constexpr iterator end() noexcept {
		return iterator(size(), str.data() + str.size());
	}

	constexpr const_iterator end() const noexcept {
		return const_iterator(size(), str.data() + str.size());
	}

	constexpr const_iterator cend() const noexcept {
		return const_iterator(size(), str.data() + str.size());
	}

	constexpr reverse_iterator rend() noexcept {
		return reverse_iterator(begin());
	}

	constexpr const_reverse_iterator rend() const noexcept {
		return const_reverse_iterator(begin());
	}

	constexpr const_reverse_iterator crend() const noexcept {
		return const_reverse_iterator(cbegin());
	}


	constexpr bool empty() const noexcept {
		return str.empty();
	}

	constexpr SizeT size() const noexcept {

		if constexpr (Unicode::isUTF32<E>()) {
			return str.size();
		} else {
			return Base::totalCodepoints;
		}

	}

	constexpr SizeT length() const noexcept {
		return size();
	}

	constexpr SizeT maxSize() const noexcept {
		return str.max_size();
	}

	constexpr void reserve(SizeT newCapacity) {
		str.reserve(newCapacity);
	}

	constexpr SizeT rawSize() const noexcept {
		return str.size();
	}

	constexpr SizeT capacity() const noexcept {
		return str.capacity();
	}

	constexpr void shrinkToFit() {
		str.shrink_to_fit();
	}


	constexpr void clear() noexcept {

		str.clear();

		if constexpr (!Unicode::isUTF32<E>()) {

			Base::distances.clear();
			Base::totalCodepoints = 0;

		}

	}

	constexpr UnicodeString& insert(SizeT index, Codepoint codepoint) {
		return insert(index, 1, codepoint);
	}

	constexpr UnicodeString& insert(SizeT index, SizeT count, Codepoint codepoint) {

		insertCodepoint(index, codepoint, count);
		return *this;

	}

	template<Char C>
	constexpr UnicodeString& insert(SizeT index, const C* s, SizeT count) {

		std::span<const C> span {s, count};
		insert(const_iterator(index, str.data() + getInclusiveOffsetOrThrow(index)), span.begin(), span.end());

		return *this;

	}

	template<Char C>
	constexpr UnicodeString& insert(SizeT index, const C* s) {
		return insert(index, s, arraySize(s));
	}

	template<Unicode::Encoding Enc>
	constexpr UnicodeString& insert(SizeT index, const UnicodeString<Enc>& ustr) {

		insert(index, ustr.str.begin(), ustr.str.end());
		return *this;

	}

	template<Unicode::Encoding Enc>
	constexpr UnicodeString& insert(SizeT index, const UnicodeString<Enc>& ustr, SizeT ssIndex, SizeT ssSize = SizeT(-1)) {

		if (ssIndex > ustr.size()) {
			throw std::out_of_range("Insertion substring index out of range");
		}

		ssSize = clampSubsize(ssIndex, ssSize, ustr.size());

		SizeT ssStart = ustr.getInclusiveOffsetDirect(ssIndex);
		SizeT ssEnd = ustr.getInclusiveOffsetDirect(ssIndex + ssSize);

		insert(index, ustr.str.data() + ssStart, ustr.str.data() + ssEnd);

		return *this;

	}

	constexpr iterator insert(const const_iterator& pos, Codepoint codepoint) {
		return insert(pos, 1, codepoint);
	}

	constexpr iterator insert(const const_iterator& pos, SizeT count, Codepoint codepoint) {

		SizeT cpIdx = pos.getCodepointIndex();

		//The container storage might get reallocated so get the distance to the start
		SizeT distance = pos.getPointer() - str.data();

		insertCodepoint(cpIdx, codepoint, count);

		//Construct a new iterator to cpIdx and return it
		return iterator(cpIdx, str.data() + distance);

	}

	template<std::input_iterator InputIt>
	constexpr iterator insert(SizeT index, const InputIt& first, const InputIt& last) {
		return insert(const_iterator(index, str.data() + getInclusiveOffsetOrThrow(index)), first, last);
	}

	template<std::input_iterator InputIt>
	constexpr iterator insert(const const_iterator& pos, const InputIt& first, const InputIt& last) {
		return insertByRange(pos, first, last);
	}

	template<Char C>
	constexpr iterator insert(const const_iterator& pos, std::initializer_list<C> list) {
		return insert(pos, list.begin(), list.end());
	}

	template<class StringView>
	constexpr UnicodeString& insert(SizeT index, const StringView& sv) requires StringViewConvertible<StringView> {

		StringViewType<StringView> view = sv;
		insert(index, view.begin(), view.end());

		return *this;

	}

	template<class StringView>
	constexpr UnicodeString& insert(SizeT index, const StringView& sv, SizeT svIndex, SizeT svSize = SizeT(-1)) requires StringViewConvertible<StringView> {

		StringViewType<StringView> view = sv;
		view = view.substr(svIndex, svSize);

		insert(index, view.begin(), view.end());

		return *this;

	}


	constexpr UnicodeString& erase(SizeT index, SizeT count = SizeT(-1)) {

		SizeT offset = getInclusiveOffsetOrThrow(index);
		count = clampSubsize(index, count, size());

		SizeT end = getInclusiveOffsetDirect(index + count);
		str.erase(offset, end - offset);
		restoreDistanceRange(index, offset, size() - count);

		return *this;

	}

	constexpr iterator erase(const const_iterator& pos) noexcept {

		SizeT index = pos.getCodepointIndex();
		SizeT offset = pos.getPointer() - str.data();

		erase(index, 1);

		return iterator(index, str.data() + offset);

	}

	constexpr iterator erase(const const_iterator& first, const_iterator last) noexcept {

		//Clamp last to first
		if (last < first) {
			last = first;
		}

		SizeT index = first.getCodepointIndex();
		SizeT endIndex = last.getCodepointIndex();
		SizeT offset = first.getPointer() - str.data();

		erase(index, endIndex - index);

		return iterator(index, str.data() + offset);

	}


	constexpr void push_back(Codepoint codepoint) {

		CharT decomposed[Traits::MaxDecomposed];
		SizeT count = Unicode::encode<E>(codepoint, decomposed);
		SizeT ssize = str.size();

		str.append(decomposed, count);
		restoreDistanceRange(size(), ssize, size() + 1);

	}


	constexpr void pop_back() noexcept {
		erase(--end());
	}


	constexpr UnicodeString& append(Codepoint codepoint) {
		return append(1, codepoint);
	}

	constexpr UnicodeString& append(SizeT count, Codepoint codepoint) {

		appendCodepoint(codepoint, count);
		return *this;

	}

	template<Unicode::Encoding Enc>
	constexpr UnicodeString& append(const UnicodeString<Enc>& ustr) {
		return append(ustr.str.begin(), ustr.str.end());
	}

	template<Unicode::Encoding Enc>
	constexpr UnicodeString& append(const UnicodeString<Enc>& ustr, SizeT ssIndex, SizeT ssSize = SizeT(-1)) {

		if (ssIndex > ustr.size()) {
			throw std::out_of_range("Appending substring index out of range");
		}

		ssSize = clampSubsize(ssIndex, ssSize, ustr.size());

		SizeT ssStart = ustr.getInclusiveOffsetDirect(ssIndex);
		SizeT ssEnd = ustr.getInclusiveOffsetDirect(ssIndex + ssSize);

		return append(ustr.str.data() + ssStart, ustr.str.data() + ssEnd);

	}

	template<Char C>
	constexpr UnicodeString& append(const C* s, SizeT count) {

		std::span<const C> span {s, count};
		return append(span.begin(), span.end());

	}

	template<Char C>
	constexpr UnicodeString& append(const C* s) {
		return append(s, arraySize(s));
	}

	template<std::input_iterator InputIt>
	constexpr UnicodeString& append(const InputIt& first, const InputIt& last) {

		appendByRange(first, last);
		return *this;

	}

	template<Char C>
	constexpr UnicodeString& append(std::initializer_list<C> list) {
		return append(list.begin(), list.size());
	}

	template<class StringView>
	constexpr UnicodeString& append(const StringView& sv) requires StringViewConvertible<StringView> {

		StringViewType<StringView> view = sv;
		return append(view.begin(), view.end());

	}

	template<class StringView>
	constexpr UnicodeString& append(const StringView& sv, SizeT svIndex, SizeT svSize = SizeT(-1)) requires StringViewConvertible<StringView> {

		StringViewType<StringView> view = sv;
		view = view.substr(svIndex, svSize);

		return append(view.begin(), view.end());

	}

	template<Unicode::Encoding Enc>
	constexpr UnicodeString& operator+=(const UnicodeString<Enc>& ustr) {
		return append(ustr);
	}

	constexpr UnicodeString& operator+=(Codepoint codepoint) {
		return append(codepoint);
	}

	template<Char C>
	constexpr UnicodeString& operator+=(const C* s) {
		return append(s);
	}

	template<Char C>
	constexpr UnicodeString& operator+=(std::initializer_list<C> list) {
		return append(list);
	}

	template<class StringView>
	constexpr UnicodeString& operator+=(const StringView& sv) requires StringViewConvertible<StringView> {
		return append(sv);
	}


	template<Unicode::Encoding Enc>
	constexpr i32 compare(const UnicodeString<Enc>& ustr) const noexcept {
		return compareDirect(0, size(), ustr.str, ustr.size());
	}

	template<Unicode::Encoding Enc>
	constexpr i32 compare(SizeT index, SizeT count, const UnicodeString<Enc>& ustr) const {

		checkIndexInclusive(index);
		count = clampSubsize(index, count, size());

		return compareDirect(index, count, ustr.str, ustr.size());

	}

	template<Unicode::Encoding Enc>
	constexpr i32 compare(SizeT index, SizeT count, const UnicodeString<Enc>& ustr, SizeT uIndex, SizeT uCount = SizeT(-1)) const {

		checkIndexInclusive(index);
		count = clampSubsize(index, count, size());
		uCount = clampSubsize(uIndex, uCount, ustr.size());

		SizeT uOffset = ustr.getInclusiveOffsetOrThrow(uIndex);

		return compareDirect(index, count, ustr.str.data() + uOffset, uCount);

	}

	template<Char C>
	constexpr i32 compare(const C* s) const noexcept {
		return compareDirect(0, size(), s, arraySize(s));
	}

	template<Char C>
	constexpr i32 compare(SizeT index, SizeT count, const C* s) const {
		return compare(index, count, s, arraySize(s));
	}

	template<Char C>
	constexpr i32 compare(SizeT index, SizeT count, const C* s, SizeT sCount) const {

		checkIndexInclusive(index);
		clampSubsize(index, count, size());

		return compareDirect(index, count, s, sCount);

	}

	template<class StringView>
	constexpr i32 compare(const StringView& sv) const noexcept(std::is_nothrow_convertible_v<const StringView&, StringViewType<StringView>>) requires StringViewConvertible<StringView> {
		return compareViewDirect(0, size(), sv);
	}

	template<class StringView>
	constexpr i32 compare(SizeT index, SizeT count, const StringView& sv) const requires StringViewConvertible<StringView> {

		checkIndexInclusive(index);
		count = clampSubsize(index, count, size());

		return compareViewDirect(index, count, sv);

	}

	template<class StringView>
	constexpr i32 compare(SizeT index, SizeT count, const StringView& sv, SizeT sIndex, SizeT sCount) const requires StringViewConvertible<StringView> {

		checkIndexInclusive(index);
		count = clampSubsize(index, count, size());

		StringViewType<StringView> view = sv;
		view = view.substr(sIndex, sCount);

		return compareViewDirect(index, count, view);

	}

	template<Char C>
	constexpr bool startsWith(std::basic_string_view<C> sv) const noexcept {
		//TODO
		return true;
	}

	//TODO

	template<Unicode::Encoding Enc>
	constexpr UnicodeString& replace(SizeT index, SizeT count, const UnicodeString<Enc>& ustr) {
		return replace(index, count, ustr.str.begin(), ustr.str.end());
	}

	template<Unicode::Encoding Enc>
	constexpr UnicodeString& replace(const const_iterator& first, const const_iterator& last, const UnicodeString<Enc>& ustr) {
		return replace(first, last, ustr.str.begin(), ustr.str.end());
	}

	template<Unicode::Encoding Enc>
	constexpr UnicodeString& replace(SizeT index, SizeT count, const UnicodeString<Enc>& ustr, SizeT uIndex, SizeT uCount = SizeT(-1)) {

		SizeT uOffset = ustr.getInclusiveOffsetOrThrow(uIndex);
		uCount = clampSubsize(uIndex, uCount, ustr.size());

		return replace(index, count, ustr.str.data() + uOffset, ustr.str.data() + uOffset + uCount);

	}

	template<std::input_iterator InputIt>
	constexpr UnicodeString& replace(SizeT index, SizeT count, const InputIt& iFirst, const InputIt& iLast) {

		count = clampSubsize(index, count, size());

		const CharT* pStart = str.data() + getInclusiveOffsetOrThrow(index);
		const CharT* pEnd = str.data() + getInclusiveOffsetOrThrow(index + count);

		return replace(const_iterator(index, pStart), const_iterator(index + count, pEnd), iFirst, iLast);

	}

	template<std::input_iterator InputIt>
	constexpr UnicodeString& replace(const const_iterator& first, const const_iterator& last, const InputIt& iFirst, const InputIt& iLast) {

		replaceByRange(first, last, iFirst, iLast);
		return *this;

	}

	template<Char C>
	constexpr UnicodeString& replace(SizeT index, SizeT count, const C* s, SizeT sCount) {

		std::span<const C> span {s, sCount};
		return replace(index, count, span.begin(), span.end());

	}

	template<Char C>
	constexpr UnicodeString& replace(const const_iterator& first, const const_iterator& last, const C* s, SizeT sCount) {

		std::span<const C> span {s, sCount};
		return replace(first, last, span.begin(), span.end());

	}

	template<Char C>
	constexpr UnicodeString& replace(SizeT index, SizeT count, const C* s) {
		return replace(index, count, s, arraySize(s));
	}

	template<Char C>
	constexpr UnicodeString& replace(const const_iterator& first, const const_iterator& last, const C* s) {
		return replace(first, last, s, arraySize(s));
	}

	constexpr UnicodeString& replace(SizeT index, SizeT count, Codepoint codepoint) {
		return replace(index, count, 1, codepoint);
	}

	constexpr UnicodeString& replace(SizeT index, SizeT count, SizeT cpCount, Codepoint codepoint) {

		count = clampSubsize(index, count, size());

		const CharT* pStart = str.data() + getInclusiveOffsetOrThrow(index);
		const CharT* pEnd = str.data() + getInclusiveOffsetOrThrow(index + count);

		return replace(const_iterator(index, pStart), const_iterator(index + count, pEnd), cpCount, codepoint);

	}

	constexpr UnicodeString& replace(const const_iterator& first, const const_iterator& last, Codepoint codepoint) {
		return replace(first, last, 1, codepoint);
	}

	constexpr UnicodeString& replace(const const_iterator& first, const const_iterator& last, SizeT cpCount, Codepoint codepoint) {

		replaceCodepoint(first, last, codepoint, cpCount);
		return *this;

	}

	template<Char C>
	constexpr UnicodeString& replace(const const_iterator& first, const const_iterator& last, std::initializer_list<C> list) {
		return replace(first, last, list.begin(), list.end());
	}

	template<class StringView>
	constexpr UnicodeString& replace(SizeT index, SizeT count, const StringView& sv) requires StringViewConvertible<StringView> {

		StringViewType<StringView> view = sv;
		return replace(index, count, view.begin(), view.end());

	}

	template<class StringView>
	constexpr UnicodeString& replace(const const_iterator& first, const const_iterator& last, const StringView& sv) requires StringViewConvertible<StringView> {

		StringViewType<StringView> view = sv;
		return replace(first, last, view.begin(), view.end());

	}

	template<class StringView>
	constexpr UnicodeString& replace(SizeT index, SizeT count, const StringView& sv, SizeT sIndex, SizeT sCount = SizeT(-1)) requires StringViewConvertible<StringView> {

		StringViewType<StringView> view = sv;
		view = view.substr(sIndex, sCount);

		return replace(index, count, view.begin(), view.end());

	}

	constexpr UnicodeString substr(SizeT pos, SizeT count = SizeT(-1)) {
		return UnicodeString(*this, pos, count);
	}

	constexpr SizeT copy(CharT* dstArray, SizeT count, SizeT pos = 0) {

		if (pos > size()) {
			throw std::out_of_range("Starting position out of bounds");
		}

		count = clampSubsize(pos, count, size());

		SizeT start = getInclusiveOffsetDirect(pos);
		SizeT end = getInclusiveOffsetDirect(pos + count);
		SizeT total = end - start;

		std::copy_n(str.data() + start, total, dstArray);

		return total;

	}

	constexpr void resize(SizeT count, SizeT units = Traits::MaxDecomposed) {

		SizeT prevSize = size();
		SizeT prevStart = str.size();

		str.resize(count * units);
		//restoreDistanceRange(prevSize, prevStart, prevSize + c)

	}

	constexpr void resizeBytes(SizeT count) {
		str.resize(count);
	}

	constexpr void swap(UnicodeString<E>& other) noexcept(noexcept(str.swap(other.str)) && noexcept(Base::distances.swap(other.distances))) {

		str.swap(other.str);

		if constexpr (!Unicode::isUTF32<E>()) {

			Base::distances.swap(other.distances);
			std::swap(Base::totalCodepoints, other.totalCodepoints);

		}

	}

private:

	constexpr void checkIndexExclusive(SizeT index) const {

		if (index >= size()) {
			throw std::out_of_range("Codepoint index out of range");
		}

	}

	constexpr void checkIndexInclusive(SizeT index) const {

		if (index > size()) {
			throw std::out_of_range("Codepoint index out of range");
		}

	}

	template<bool Verify>
	constexpr SizeT getOffset(SizeT index) const noexcept(!Verify) {

		if constexpr (Verify) {
			checkIndexExclusive(index);
		}

		if constexpr (Unicode::isUTF8<E>()) {

			//Forward iteration for UTF-8 is much faster since the first byte carries information about the whole codepoint size
			SizeT dstIdx = index / Base::CPDistance;

			//Offset to traverse
			SizeT traversalOffset = index % Base::CPDistance;

			//Start pointer to iterate from
			const CharT* startPtr = &str[Base::distances[dstIdx]];

			for(SizeT i = 0; i < traversalOffset; i++) {
				startPtr += Unicode::getEncodedSize<E>(startPtr);
			}

			return startPtr - str.data();

		} else if constexpr (Unicode::isUTF16<E>()) {

			//For UTF-16, bidirectional iteration is equally performant because surrogates are reflected in both code units
			//Therefore, round the index
			SizeT dstIdx = (index + Base::CPDistance / 2) / Base::CPDistance;
			TT::MakeSigned<SizeT> offset = index - dstIdx * Base::CPDistance;

			//If the distance index is out of bounds (i.e. at the border with negative offsets), switch to forward traversal
			if (dstIdx >= Base::distances.size()) {

				dstIdx--;
				offset += Base::CPDistance;

			}

			const CharT* startPtr = &str[Base::distances[dstIdx]];

			if (offset >= 0) {

				for (SizeT i = 0; i < offset; i++) {
					startPtr += Unicode::getEncodedSize<E>(startPtr);
				}

			} else {

				for (imax i = 0; i > offset; i--) {
					startPtr -= Unicode::getEncodedSize<E>(startPtr);
				}

			}

			return startPtr - str.data();

		} else {

			return index;

		}

	}

	template<bool Verify>
	constexpr SizeT getInclusiveOffset(SizeT index) const noexcept(!Verify) {

		if (index == size()) {
			return str.size();
		} else {
			return getOffset<Verify>(index);
		}

	}

	constexpr SizeT getOffsetOrThrow(SizeT index) const {
		return getOffset<true>(index);
	}

	constexpr SizeT getOffsetDirect(SizeT index) const noexcept {
		return getOffset<false>(index);
	}

	constexpr SizeT getInclusiveOffsetOrThrow(SizeT index) const {
		return getInclusiveOffset<true>(index);
	}

	constexpr SizeT getInclusiveOffsetDirect(SizeT index) const noexcept {
		return getInclusiveOffset<false>(index);
	}


	constexpr static SizeT clampSubsize(SizeT index, SizeT size, SizeT clamp) noexcept {

		if (size > clamp - index) {
			return clamp - index;
		}

		return size;

	}

	/*
	 *  Returns the length of the char array, in UTF chars
	 */
	template<Char C>
	constexpr static SizeT arraySize(const C* s) noexcept {

		const C* p = s;

		while (*p) {
			p++;
		}

		return p - s;

	}


	constexpr void restoreDistanceRange(SizeT cpStart, SizeT charStart, SizeT totalCps) {

		if constexpr (!Unicode::isUTF32<E>()) {

			//First, we get the start index into distances to modify
			SizeT dstOffset = cpStart / Base::CPDistance;

			//Resize to fit new distance elements
			Base::totalCodepoints = totalCps;
			Base::distances.resize((totalCps + Base::CPDistance - 1) / Base::CPDistance);

			SizeT x = cpStart;
			const CharT* ptr = str.data() + charStart;

			//Calculate all new offsets from start point onwards
			for(SizeT i = dstOffset + 1; i < Base::distances.size(); i++) {

				for(; x < i * Base::CPDistance; x++) {
					ptr += Unicode::getEncodedSize<E>(ptr);
				}

				Base::distances[i] = ptr - str.data();

			}

		}

	}

	/*
	 *  Fast back pointer access (operator[size() - 1] is slower due to random access nature)
	 */
	constexpr const CharT* fastBack() const noexcept {

		const CharT* backPtr = str.data() + str.size();
		backPtr -= Unicode::getEncodedSizeBackwards<E>(backPtr);

		return backPtr;

	}


	template<std::input_iterator InputIt>
	constexpr SizeT calculateRangeUnitCount(InputIt first, const InputIt& last, SizeT& cps) {

		constexpr Unicode::Encoding Enc = Unicode::TypeEncoding<TT::RemoveCVRef<decltype(*first)>>;
		SizeT units = 0;
		SizeT codepoints = 0;

		if constexpr (std::contiguous_iterator<InputIt>) {

			//Contiguous iterators allow examining the underlying memory effectively
			while (first < last) {

				units += Unicode::getTranscodedSize<Enc, E>(&*first);
				first += Unicode::getEncodedSize<Enc>(&*first);
				codepoints++;

			}

		} else {

			//All other iterators require step-wise decomposition before the size can be calculated
			typename Unicode::UTFEncodingTraits<Enc>::Type decomposed[Unicode::UTFEncodingTraits<Enc>::MaxDecomposed];
			SizeT n = 0;

			while (first != last) {

				decomposed[0] = *first++;
				n = Unicode::getEncodedSize<Enc>(decomposed);

				for(SizeT i = 1; i < n; i++) {
					decomposed[i] = *first++;
				}

				units += Unicode::getTranscodedSize<Enc, E>(decomposed);
				codepoints++;

			}

		}

		cps = codepoints;

		return units;

	}

	template<std::input_iterator InputIt>
	constexpr SizeT calculateRangeUnitCount(InputIt first, const InputIt& last) {

		[[maybe_unused]] SizeT cps;
		return calculateRangeUnitCount(std::move(first), last, cps);

	}

	template<std::input_iterator InputIt>
	constexpr SizeT inplaceTranscode(InputIt first, const InputIt& last, CharT* dest) {

		constexpr Unicode::Encoding Enc = Unicode::TypeEncoding<TT::RemoveCVRef<decltype(*first)>>;
		SizeT transcodedCodepoints = 0;

		if constexpr (std::contiguous_iterator<InputIt>) {

			while (first != last) {

				SizeT srcN;
				SizeT destN = Unicode::transcode<Enc, E>(&*first, dest, srcN);

				first += srcN;
				dest += destN;
				transcodedCodepoints++;

			}

		} else {

			typename Unicode::UTFEncodingTraits<Enc>::Type decomposed[Unicode::UTFEncodingTraits<Enc>::MaxDecomposed];
			SizeT n = 0;

			while (first != last) {

				decomposed[0] = *first++;
				n = Unicode::getEncodedSize<Enc>(decomposed);

				for(SizeT i = 1; i < n; i++) {
					decomposed[i] = *first++;
				}

				Unicode::transcode<Enc, E>(decomposed, dest);
				transcodedCodepoints++;

			}

		}

		return transcodedCodepoints;

	}

	constexpr void replaceResize(SizeT endOffset, SizeT units, SizeT replaceUnits) {

		if (replaceUnits > units) {

			//String has to grow
			SizeT delta = replaceUnits - units;
			str.resize(str.size() + delta);
			std::copy_backward(str.begin() + endOffset, str.end() - delta, str.end());

		} else if (replaceUnits < units) {

			//String has to shrink
			SizeT delta = units - replaceUnits;
			std::copy(str.begin() + endOffset, str.end(), str.begin() + endOffset - delta);
			str.resize(str.size() - delta);

		}

	}


	constexpr void copyDirect(const UnicodeString<E>& ustr) {

		if constexpr (!Unicode::isUTF32<E>()) {

			Base::distances = ustr.distances;
			Base::totalCodepoints = ustr.totalCodepoints;

		}

		str = ustr.str;

	}


	constexpr void moveDirect(UnicodeString<E>&& ustr) noexcept(std::is_nothrow_move_assignable_v<decltype(str)> && std::is_nothrow_move_assignable_v<decltype(Base::distances)>) {

		if constexpr (!Unicode::isUTF32<E>()) {

			Base::distances = std::move(ustr.distances);
			Base::totalCodepoints = ustr.totalCodepoints;

		}

		str = std::move(ustr.str);

	}


	constexpr void assignCodepoint(Codepoint codepoint, SizeT n) {

		if constexpr (!Unicode::isUTF32<E>()) {

			//Decompose codepoint
			CharT decomposed[Traits::MaxDecomposed];
			SizeT count = Unicode::encode<E>(codepoint, decomposed);

			//Resize array
			str.resize(count * n);

			//Assign
			for(SizeT i = 0; i < n; i++) {
				std::copy_n(decomposed, count, str.begin() + i * count);
			}

			restoreDistanceRange(0, 0, n);

		} else {

			str.assign(n, codepoint);

		}

	}


	template<std::input_iterator InputIt>
	constexpr void assignByRange(const InputIt& first, const InputIt& last) {

		constexpr Unicode::Encoding Enc = Unicode::TypeEncoding<TT::RemoveCVRef<decltype(*first)>>;

		if constexpr (E != Enc || !Unicode::isUTF32<E>()) {

			//Precalculate the total insertion size
			SizeT totalCodepoints;
			SizeT totalSegments = calculateRangeUnitCount(first, last, totalCodepoints);

			//Resize array
			str.resize(totalSegments);

			//Assign transcoded range
			inplaceTranscode(first, last, str.data());
			restoreDistanceRange(0, 0, totalCodepoints);

		} else {

			str.assign(first, last);

		}

	}


	constexpr void insertCodepoint(SizeT index, Codepoint codepoint, SizeT n) {

		if constexpr (!Unicode::isUTF32<E>()) {

			SizeT offset = getInclusiveOffsetOrThrow(index);

			//Decompose codepoint
			CharT decomposed[Traits::MaxDecomposed];
			SizeT count = Unicode::encode<E>(codepoint, decomposed);

			//Resize array
			SizeT prevSize = str.size();
			str.resize(prevSize + count * n);

			//Copy elements to the new end
			std::copy_backward(str.begin() + offset, str.begin() + prevSize, str.end());

			//Now insert the new decomposed codepoint(s)
			for(SizeT i = 0; i < n; i++) {
				std::copy_n(decomposed, count, str.begin() + offset + i * count);
			}

			restoreDistanceRange(index, offset, Base::totalCodepoints + n);

		} else {

			str.insert(index, n, codepoint);

		}

	}

	template<std::input_iterator InputIt>
	constexpr iterator insertByRange(const const_iterator& pos, const InputIt& first, const InputIt& last) {

		constexpr Unicode::Encoding Enc = Unicode::TypeEncoding<TT::RemoveCVRef<decltype(*first)>>;

		if constexpr (E != Enc || !Unicode::isUTF32<E>()) {

			SizeT index = pos.getCodepointIndex();

			//Precalculate the total insertion size
			SizeT totalSegments = calculateRangeUnitCount(first, last);

			//Resize array
			SizeT offset = pos.getPointer() - str.data();
			SizeT prevSize = str.size();
			str.resize(prevSize + totalSegments);

			//Copy elements to the new end
			std::copy_backward(str.begin() + offset, str.begin() + prevSize, str.end());

			//Now insert the new decomposed codepoint(s) over the range
			CharT* ptr = str.data() + offset;
			SizeT totalCodepoints = size();

			totalCodepoints += inplaceTranscode(first, last, ptr);
			restoreDistanceRange(index, offset, totalCodepoints);

			return iterator(index, ptr);

		} else {

			return iterator(pos.getCodepointIndex(), &*str.insert(pos, first, last));

		}

	}


	constexpr void appendCodepoint(Codepoint codepoint, SizeT n) {

		if constexpr (!Unicode::isUTF32<E>()) {

			//Decompose codepoint
			CharT decomposed[Traits::MaxDecomposed];
			SizeT count = Unicode::encode<E>(codepoint, decomposed);

			//Resize array
			SizeT prevSize = str.size();
			str.resize(prevSize + count * n);

			//Now insert the new decomposed codepoint(s)
			for(SizeT i = 0; i < n; i++) {
				std::copy_n(decomposed, count, str.begin() + prevSize + i * count);
			}

			SizeT prevCpCount = size();
			restoreDistanceRange(prevCpCount, prevSize, prevCpCount + n);

		} else {

			str.append(n, codepoint);

		}

	}

	template<std::input_iterator InputIt>
	constexpr void appendByRange(const InputIt& first, const InputIt& last) {

		constexpr Unicode::Encoding Enc = Unicode::TypeEncoding<TT::RemoveCVRef<decltype(*first)>>;

		if constexpr (E != Enc || !Unicode::isUTF32<E>()) {

			//Precalculate the total insertion size
			SizeT totalSegments = calculateRangeUnitCount(first, last);

			//Resize array
			SizeT prevSize = str.size();
			str.resize(prevSize + totalSegments);

			//Now insert the new decomposed codepoint(s) over the range
			CharT* ptr = str.data() + prevSize;
			SizeT totalCodepoints = size();

			totalCodepoints += inplaceTranscode(first, last, ptr);
			restoreDistanceRange(size(), prevSize, totalCodepoints);

		} else {

			str.append(first, last);

		}

	}

	template<Char C>
	constexpr i32 compareDirect(SizeT index, SizeT count, const C* s, SizeT sSize) const noexcept {
		//TODO
		return 0;
	}

	template<Char C>
	constexpr i32 compareViewDirect(SizeT index, SizeT count, const std::basic_string_view<C>& view) const noexcept {
		return compareDirect(index, count, view.data(), view.size());
	}

	constexpr void replaceCodepoint(const const_iterator& start, const const_iterator& end, Codepoint codepoint, SizeT n) {

		if constexpr (!Unicode::isUTF32<E>()) {

			SizeT index = start.getCodepointIndex();
			SizeT cpDelta = end.getCodepointIndex() - index;

			CharT decomposed[Traits::MaxDecomposed];
			SizeT encodedSize = Unicode::encode<E>(codepoint, decomposed);
			SizeT replaceUnits = encodedSize * n;

			SizeT startOffset = start.getPointer() - str.data();
			SizeT endOffset = end.getPointer() - str.data();
			SizeT units = endOffset - startOffset;

			replaceResize(endOffset, units, replaceUnits);

			//Now insert the new decomposed codepoint(s)
			for(SizeT i = 0; i < n; i++) {
				std::copy_n(decomposed, encodedSize, str.data() + startOffset + i * encodedSize);
			}

			restoreDistanceRange(index, startOffset, size() + n - cpDelta);

		} else {

			str.replace(start.getPointer(), end.getPointer(), n, codepoint);

		}

	}

	template<std::input_iterator InputIt>
	constexpr void replaceByRange(const const_iterator& start, const const_iterator& end, const InputIt& first, const InputIt& last) {

		constexpr Unicode::Encoding Enc = Unicode::TypeEncoding<TT::RemoveCVRef<decltype(*first)>>;

		if constexpr (E != Enc || !Unicode::isUTF32<E>()) {

			SizeT startIndex = start.getCodepointIndex();
			SizeT startOffset = start.getPointer() - str.data();
			SizeT endOffset = end.getPointer() - str.data();
			SizeT units = endOffset - startOffset;

			//Calculate transcoding units
			SizeT replaceCPs = 0;
			SizeT replaceUnits = calculateRangeUnitCount(first, last, replaceCPs);

			replaceResize(endOffset, units, replaceUnits);

			//Replace through overwrite
			CharT* ptr = str.data() + startOffset;
			SizeT transcodedCPs = inplaceTranscode(first, last, ptr);

			restoreDistanceRange(startIndex, startOffset, size() + transcodedCPs - replaceCPs);

		} else {

			str.replace(start.getCodepointIndex(), end.getCodepointIndex() - start.getCodepointIndex(), first, last);

		}

	}


	Container str;

};


using U8String = UnicodeString<Unicode::UTF8>;
using U16String = UnicodeString<Unicode::UTF16>;
using U32String = UnicodeString<Unicode::UTF32>;