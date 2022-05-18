/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 bitspan.hpp
 */

#pragma once

#include "common/typetraits.hpp"
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
class BitSpan : private BitSpanBase<Extent == SizeT(-1)> {

	consteval static SizeT getSubspanExtent(SizeT ThisExtent, SizeT Offset, SizeT Count) noexcept {

		if (Count != std::dynamic_extent) {
			return Count;
		} else if (ThisExtent != std::dynamic_extent) {
			return ThisExtent - Offset;
		} else {
			return std::dynamic_extent;
		}

	}

	template<class T>
	constexpr static auto convertTToPointer(T&& t) noexcept {

		using U = TT::Decay<T>;

		if constexpr (CC::Equal<TT::RemoveCV<TT::RemovePointer<U>>, u8>) {

			if constexpr (CC::PointerType<U>) {
				return t;
			} else {
				return &t;
			}

		} else if constexpr (CC::PointerType<U>) {
			return Bits::toByteArray(t);
		} else {
			return Bits::toByteArray(std::addressof(t));
		}

	}

public:

	//Definition after template default parameter to keep it scoped
	constexpr static bool Dynamic = Extent == std::dynamic_extent;
	constexpr static SizeT SpanExtent = Extent;

	using Base = BitSpanBase<Dynamic>;

	using P = TT::ConditionalConst<Const, u8>;


	class BitProxy {

	public:

		constexpr BitProxy(P* ptr, u8 bit) noexcept : ptr(ptr), bit(bit) {}

		constexpr BitProxy& operator=(const BitProxy& rhs) noexcept {

			*this = bool(rhs);
			return *this;

		}

		constexpr BitProxy& operator=(bool b) noexcept requires (!Const) {

			*ptr = (*ptr & ~(1 << bit)) | (static_cast<u8>(!!b) << bit);
			return *this;

		}

		constexpr operator bool() const noexcept {
			return *ptr & (1 << bit);
		}

		constexpr P* operator&() const noexcept {
			return ptr;
		}

	private:

		P* ptr;
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
		constexpr Iterator(P* p, SizeT b) noexcept : ptr(p), bit(b) {}
		constexpr Iterator(const Iterator& it) noexcept = default;

		constexpr reference operator*() const noexcept { return BitProxy(ptr, bit); }
		constexpr pointer operator->() const noexcept { return BitProxy(ptr, bit); }

		constexpr Iterator& operator++() noexcept { advance(); return *this; }
		constexpr Iterator& operator--() noexcept { retreat(); return *this; }
		constexpr Iterator operator++(int) noexcept { auto cpy = *this; ++(*this); return cpy; }
		constexpr Iterator operator--(int) noexcept { auto cpy = *this; --(*this); return cpy; }

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

		constexpr Iterator operator+(SizeT n) const noexcept { Iterator it = *this; it.advance(n); return it; }
		constexpr Iterator operator-(SizeT n) const noexcept { Iterator it = *this; it.retreat(n); return it; }
		constexpr Iterator& operator+=(SizeT n) noexcept { advance(n); return *this; }
		constexpr Iterator& operator-=(SizeT n) noexcept { retreat(n); return *this; }
		constexpr reference operator[](SizeT n) const noexcept { return *(*this + n); }

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

		P* ptr;
		u8 bit;

	};

	using iterator = Iterator;
	using reverse_iterator = std::reverse_iterator<iterator>;


	constexpr BitSpan() noexcept requires (Extent == 0 || Dynamic) : ptr(nullptr), start(0) {}


	template<CC::PointerType T, class U = TT::RemovePointer<T>>
	constexpr BitSpan(const T& t, SizeT size = Bits::bitCount<U>()) noexcept requires (CC::ConstType<U> <= Const) : Base(size), ptr(convertTToPointer(t)), start(0) {}

	template<CC::PointerType T, class U = TT::RemovePointer<T>>
	constexpr BitSpan(const T& t, SizeT start, SizeT size) noexcept requires (CC::ConstType<U> <= Const) : Base(size), ptr(convertTToPointer(t)), start(start) { normalize(); }


	template<class T, SizeT N>
	constexpr BitSpan(T(& t)[N], SizeT size = Bits::bitCount<T>() * N) noexcept requires (CC::ConstType<T> <= Const) : Base(size), ptr(convertTToPointer(t)), start(0) {}

	template<class T, SizeT N>
	constexpr BitSpan(T(& t)[N], SizeT start, SizeT size) noexcept requires (CC::ConstType<T> <= Const) : Base(size), ptr(convertTToPointer(t)), start(start) { normalize(); }


	template<class T, SizeT N>
	constexpr explicit(!Dynamic && N == std::dynamic_extent) BitSpan(const std::span<T, N>& span) noexcept requires (CC::ConstType<T> <= Const) : Base(span.size() * sizeof(T)), ptr(convertTToPointer(span.data())), start(0) {}


	template<SizeT N, bool ConstOther>
	constexpr explicit(!Dynamic && N == std::dynamic_extent) BitSpan(const BitSpan<N, ConstOther>& other) noexcept requires ((Dynamic || N == Extent) && ConstOther <= Const) : Base(other.size()), ptr(other.ptr), start(other.start) {}

	constexpr BitSpan(const BitSpan& other) noexcept = default;


	template<SizeT N, bool ConstOther>
	constexpr BitSpan& operator=(const BitSpan<N, ConstOther>& other) noexcept requires ((Dynamic || N == Extent) && ConstOther <= Const) {

		if (other != *this) {

			if constexpr (Dynamic) {
				Base::size = other.size();
			}

			ptr = other.ptr;
			start = other.start;

		}

		return *this;

	}

	constexpr BitSpan& operator=(const BitSpan& other) noexcept = default;


	constexpr iterator begin() const noexcept {
		return iterator(ptr, start);
	}

	constexpr iterator end() const noexcept {

		P* endptr = ptr + (size() + start) / 8;
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

	constexpr P* data() const noexcept {
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


	template<CC::Arithmetic A, SizeT Size = Math::min(Bits::bitCount<A>(), Extent)> requires (Size <= Math::min(Bits::bitCount<A>(), Extent))
	constexpr A read() const noexcept {
		return read<A>(Size);
	}


	template<CC::Arithmetic A>
	constexpr A read(SizeT size) const noexcept {

		using I = TT::UnsignedFromSize<sizeof(A)>;

		if (size == 0) {
			return Bits::cast<A>(I(0));
		}

		if (size > this->size()) {

			arc_force_assert("Attempted to read data past the end of the span");
			size = this->size();

		}

		SizeT alignedReadBytes = Math::alignUp(size, 8) / 8;
		SizeT readBytes = Math::alignUp(size + start, 8) / 8;

		I i = Bits::assemble<I>(ptr, readBytes);

		i >>= start;

		if (readBytes != alignedReadBytes) {

			u8 stray = ptr[readBytes - 1];
			i |= static_cast<I>(stray) << (Bits::bitCount<I>() - start);

		}

		i = Bits::mask(i, 0, size);

		return Bits::cast<A>(i);

	}


	template<SizeT Size, CC::Arithmetic A> requires (!Const)
	constexpr void write(A a) noexcept {
		write<A, Size>(a);
	}

	template<CC::Arithmetic A, SizeT Size = Math::min(Bits::bitCount<A>(), Extent)> requires (Size <= Math::min(Bits::bitCount<A>(), Extent) && !Const)
	constexpr void write(A a) noexcept {
		write(Size, a);
	}

	template<CC::Arithmetic A> requires (!Const)
	constexpr void write(SizeT size, A a) noexcept {

		if (size == 0) {
			return;
		}

		if (size > this->size()) {

			arc_force_assert("Attempted to write data past the end of the span");
			size = this->size();

		}

		using I = TT::UnsignedFromSize<sizeof(A)>;

		I i = Bits::cast<I>(a);
		i = Bits::mask(i, 0, size);

		SizeT end = start + size;
		SizeT startBits = (8 - start) % 8;
		SizeT endBits = end % 8;

		bool collapsed = end / 8 == 0;
		SizeT byteCopies = collapsed ? 0 : (size - startBits - endBits) / 8;

		if (collapsed) {

			//First and last byte collapse
			u8 prev = ptr[0];
			prev = Bits::clear(prev, start, size);
			ptr[0] = prev | i << start;

		} else {

			//Separate start and end bytes
			u32 idx = 0;

			if (start) {

				u8 startByte = ptr[idx];
				startByte = Bits::mask(startByte, 0, start);
				startByte |= Bits::mask(i, 0, startBits) << start;
				ptr[idx++] = startByte;

				i >>= startBits;

			}

			Bits::disassemble(i, ptr + idx, byteCopies);
			idx += byteCopies;

			i >>= byteCopies * 8;

			if (endBits) {

				u8 endByte = ptr[idx];
				endByte = Bits::clear(endByte, 0, endBits);
				endByte |= Bits::mask(i, 0, endBits);
				ptr[idx] = endByte;

			}

		}

	}

private:

	constexpr void normalize() noexcept {

		ptr += start / 8;
		start %= 8;

	}


	P* ptr;
	SizeT start;

};



template<CC::PointerType T, class U = TT::RemovePointer<T>>
BitSpan(const T&) -> BitSpan<Bits::bitCount<U>(), CC::ConstType<U>>;

template<CC::PointerType T, class U = TT::RemovePointer<T>>
BitSpan(const T&, SizeT) -> BitSpan<std::dynamic_extent, CC::ConstType<U>>;

template<CC::PointerType T, class U = TT::RemovePointer<T>>
BitSpan(const T&, SizeT, SizeT) -> BitSpan<std::dynamic_extent, CC::ConstType<U>>;

template<class T, SizeT N>
BitSpan(T(&)[N]) -> BitSpan<Bits::bitCount<T>() * N, CC::ConstType<T>>;

template<class T, SizeT N>
BitSpan(T(&)[N], SizeT) -> BitSpan<std::dynamic_extent, CC::ConstType<T>>;

template<class T, SizeT N>
BitSpan(T(&)[N], SizeT, SizeT) -> BitSpan<std::dynamic_extent, CC::ConstType<T>>;

template<class T, SizeT SExtent>
BitSpan(const std::span<T, SExtent>&) -> BitSpan<SExtent == std::dynamic_extent ? std::dynamic_extent : SExtent * 8, CC::ConstType<T>>;