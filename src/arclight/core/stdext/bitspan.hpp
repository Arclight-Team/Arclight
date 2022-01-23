/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 bitspan.hpp
 */

#pragma once

#include "util/typetraits.hpp"
#include "math/traits.hpp"
#include "math/math.hpp"
#include "util/assert.hpp"
#include "util/bits.hpp"
#include "types.hpp"

#include <span>


template<bool Dynamic>
struct BitSpanBase {

	constexpr BitSpanBase() noexcept = default;
	constexpr explicit BitSpanBase(SizeT size) noexcept : size(size) {}

	SizeT size;

};

template<>
struct BitSpanBase<false> {

	constexpr BitSpanBase() noexcept = default;
	constexpr explicit BitSpanBase([[maybe_unused]] SizeT size) noexcept {}

};



template<SizeT Extent, bool Const = true>
class BitSpan : private BitSpanBase<Extent == SizeT(-1)>{

	consteval static SizeT getSubspanExtent(SizeT ThisExtent, SizeT Offset, SizeT Count) noexcept {

		if (Count != std::dynamic_extent) {
			return Count;
		} else if (ThisExtent != std::dynamic_extent) {
			return ThisExtent - Offset;
		} else {
			return std::dynamic_extent;
		}

	}

public:

	//Definition after template default parameter to keep it scoped
	constexpr static bool Dynamic = Extent == std::dynamic_extent;
	constexpr static SizeT SpanExtent = Extent;

	using Base = BitSpanBase<Dynamic>;

	using U = TT::ConditionalConst<Const, u8>;


	class BitProxy {

	public:

		constexpr BitProxy(U* ptr, u8 bit) noexcept : ptr(ptr), bit(bit) {}

		constexpr BitProxy& operator=(bool b) noexcept requires (!Const) {

			*ptr = (*ptr & ~(1 << bit)) | (static_cast<u8>(!!b) << bit);
			return *this;

		}

		constexpr operator bool() const noexcept {
			return *ptr & (1 << bit);
		}

	private:

		U* ptr;
		u8 bit;

	};


	class Iterator {

	public:

		using iterator_category = std::random_access_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = BitProxy;
		using pointer = value_type;
		using reference = value_type;
		using element_type = value_type;


		constexpr Iterator() noexcept : ptr(nullptr), bit(0) {}
		constexpr Iterator(U* p, SizeT b) noexcept : ptr(p), bit(b) {}
		constexpr Iterator(const Iterator& it) noexcept = default;

		constexpr reference operator*() const noexcept { return BitProxy(ptr, bit); }
		constexpr pointer operator->() const noexcept { return BitProxy(ptr, bit); }

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

		constexpr bool operator==(const Iterator& other) const noexcept {
			return ptr == other.ptr && bit == other.bit;
		}

		constexpr auto operator<=>(const Iterator& other) const noexcept {

			if (ptr == other.ptr) {
				return bit <=> other.bit;
			} else {
				return ptr <=> other.ptr;
			}

		}

		constexpr Iterator operator+(SizeT n) const noexcept    { Iterator it = *this; it.advance(n); return it; }
		constexpr Iterator operator-(SizeT n) const noexcept    { Iterator it = *this; it.retreat(n); return it; }
		constexpr Iterator& operator+=(SizeT n) noexcept        { advance(n); return *this; }
		constexpr Iterator& operator-=(SizeT n) noexcept        { retreat(n); return *this; }
		constexpr reference operator[](SizeT n) const noexcept  { return *(*this + n); }

		constexpr difference_type operator-(const Iterator& other) const noexcept { return (ptr - other.ptr) * 8 + (bit - other.bit); }
		friend constexpr Iterator operator+(SizeT n, const Iterator& it) noexcept { return it + n; }

	private:

		constexpr void advance() noexcept {
			return advance(1);
		}

		constexpr void retreat() noexcept {
			return retreat(1);
		}

		constexpr void advance(SizeT n) noexcept {

			ptr += n / 8;
			bit += n % 8;

			if (bit >= 8) {
				ptr++;
				bit -= 8;
			}

		}

		constexpr void retreat(SizeT n) noexcept {

			ptr -= n / 8;
			u8 leftBits = n % 8;

			if (leftBits > bit) {
				ptr--;
				bit += 8 - leftBits;
			} else {
				bit -= leftBits;
			}

		}

		U* ptr;
		u8 bit;

	};

	using iterator = Iterator;
	using reverse_iterator = std::reverse_iterator<iterator>;


	constexpr BitSpan() noexcept requires (Extent == 0 || Dynamic) : ptr(nullptr), start(0) {}

	template<class T, class U = TT::RemovePointer<T>>
	constexpr explicit(!Dynamic) BitSpan(T& t, SizeT start = 0, SizeT size = Bits::bitCount<U>()) noexcept requires ((PointerType<T> || Enum<T> || Arithmetic<T>) && ConstType<U> <= Const) : Base(size), ptr(Bits::toByteArray([&]() { if constexpr (PointerType<T>) { return t; } else { return std::addressof(t); }}())), start(start) { normalize(); }

	template<class T, class U = TT::RemovePointer<T>>
	constexpr explicit(!Dynamic) BitSpan(const T& t, SizeT start = 0, SizeT size = Bits::bitCount<U>()) noexcept requires (PointerType<T> && ConstType<U> <= Const) : Base(size), ptr(Bits::toByteArray(t)), start(start) { normalize(); }

	template<class T, SizeT N>
	constexpr explicit(!Dynamic) BitSpan(T(&t)[N], SizeT start = 0, SizeT size = Bits::bitCount<T>() * N) noexcept requires (ConstType<T> <= Const) : Base(size), ptr(Bits::toByteArray(t)), start(start) { normalize(); }

	template<class T>
	constexpr explicit(!Dynamic) BitSpan(const std::span<T>& span) noexcept requires (ConstType<T> <= Const) : Base(span.size() * sizeof(T)), ptr(Bits::toByteArray(span.data())), start(0) {}


	template<SizeT N, bool ConstOther>
	constexpr BitSpan(const BitSpan<N, ConstOther>& other) noexcept requires ((Dynamic || N == Extent) && ConstOther <= Const) : Base(other.size()), ptr(other.ptr), start(other.start) {}

	constexpr BitSpan(const BitSpan& other) noexcept = default;


	constexpr BitSpan& operator=(const BitSpan& other) noexcept = default;


	constexpr iterator begin() const noexcept {
		return iterator(ptr, start);
	}

	constexpr iterator end() const noexcept {

		U* endptr = ptr + (size() + start) / 8;
		u8 bits = (size() + start) % 8;

		return iterator(endptr, bits);

	}

	constexpr reverse_iterator rbegin() const noexcept {
		return reverse_iterator(end());
	}

	constexpr reverse_iterator rend() const noexcept {
		return reverse_iterator(begin());
	}


	constexpr auto front() const noexcept {
		return *begin();
	}

	constexpr auto back() const noexcept {
		return *(end() - 1);
	}

	constexpr auto operator[](SizeT idx) const {
		return begin()[idx];
	}

	constexpr U* data() const noexcept {
		return ptr;
	}

	constexpr SizeT size() const noexcept {

		if constexpr (Dynamic) {
			return Base::size;
		} else {
			return Extent;
		}

	}

	constexpr SizeT sizeBytes() const noexcept {
		return Math::alignUp(size(), 8) / 8;
	}

	constexpr bool empty() const noexcept {
		return !size();
	}

	template<SizeT Count>
	constexpr BitSpan<Count, Const> first() const noexcept requires (Count != std::dynamic_extent && Count <= Extent) {

		if constexpr (Dynamic) {
			arc_assert(Count <= size(), "Illegal subspan from smaller BitSpan");
		}

		return BitSpan<Count, Const>(ptr, start, Count);

	}

	constexpr BitSpan<std::dynamic_extent, Const> first(SizeT Count) const noexcept {

		Count = Math::min(size(), Count);
		return BitSpan<std::dynamic_extent, Const>(ptr, start, Count);

	}

	template<SizeT Count>
	constexpr BitSpan<Count, Const> last() const noexcept requires (Count != std::dynamic_extent && Count <= Extent) {

		if constexpr (Dynamic) {
			arc_assert(Count <= size(), "Illegal subspan from smaller BitSpan");
		}

		return BitSpan<Count, Const>(ptr, start + size() - Count, Count);

	}

	constexpr BitSpan<std::dynamic_extent, Const> last(SizeT Count) const noexcept {

		Count = Math::min(size(), Count);
		return BitSpan<std::dynamic_extent, Const>(ptr, start + size() - Count, Count);

	}

	template<SizeT Offset, SizeT Count = std::dynamic_extent, SizeT NewExtent = getSubspanExtent(Extent, Offset, Count)>
	constexpr BitSpan<NewExtent, Const> subspan() const noexcept requires (Offset <= Extent && (Count == std::dynamic_extent || Count <= (Extent - Offset))) {

		if constexpr (Count != std::dynamic_extent) {
			arc_assert(Count <= size() - Offset, "Illegal subspan from smaller BitSpan");
		}

		return BitSpan<NewExtent, Const>(ptr, start + Offset, Count == std::dynamic_extent ? size() - Offset : Count);

	}

	constexpr BitSpan<std::dynamic_extent, Const> subspan(SizeT Offset, SizeT Count = std::dynamic_extent) const noexcept {

		if (Offset > size()) {
			return BitSpan<std::dynamic_extent, Const>();
		}

		Count = Math::min(size() - Offset, Count);
		return BitSpan<std::dynamic_extent, Const>(ptr, start + Offset, Count);

	}


	constexpr BitSpan<Extent, true> toConstSpan() const noexcept {
		return BitSpan<Extent, true>(ptr, start, size());
	}


private:

	constexpr void normalize() noexcept {

		ptr += start / 8;
		start %= 8;

	}


	U* ptr;
	SizeT start;

};


template<class T, class U = TT::RemovePointer<T>>
BitSpan(T&) -> BitSpan<Bits::bitCount<U>(), ConstType<U>>;

template<class T, class U = TT::RemovePointer<T>>
BitSpan(T&, SizeT) -> BitSpan<Bits::bitCount<U>(), ConstType<U>>;

template<class T, class U = TT::RemovePointer<T>>
BitSpan(T&, SizeT, SizeT) -> BitSpan<std::dynamic_extent, ConstType<U>>;

template<class T, class U = TT::RemovePointer<T>>
BitSpan(const T&) -> BitSpan<Bits::bitCount<U>(), ConstType<U>>;

template<class T, class U = TT::RemovePointer<T>>
BitSpan(const T&, SizeT) -> BitSpan<Bits::bitCount<U>(), ConstType<U>>;

template<class T, class U = TT::RemovePointer<T>>
BitSpan(const T&, SizeT, SizeT) -> BitSpan<std::dynamic_extent, ConstType<U>>;

template<class T, SizeT N>
BitSpan(T(&)[N]) -> BitSpan<Bits::bitCount<T>() * N, ConstType<T>>;

template<class T, SizeT N>
BitSpan(T(&)[N], SizeT) -> BitSpan<Bits::bitCount<T>() * N, ConstType<T>>;

template<class T, SizeT N>
BitSpan(T(&)[N], SizeT, SizeT) -> BitSpan<std::dynamic_extent, ConstType<T>>;

template<class T, SizeT SExtent>
BitSpan(const std::span<T, SExtent>&) -> BitSpan<SExtent == std::dynamic_extent ? std::dynamic_extent : SExtent * 8, ConstType<T>>;