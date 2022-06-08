/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 range.hpp
 */

#pragma once

#include "common/concepts.hpp"
#include "common/typetraits.hpp"

#include <iterator>



template<CC::Arithmetic T, CC::Arithmetic StepT>
class Range {

public:

	class Iterator;

	struct Sentinel {

		constexpr explicit Sentinel(T end) noexcept : end(end) {}

		T end;

	};

	class Iterator {

	public:

		using iterator_category = std::bidirectional_iterator_tag;
		using value_type = T;
		using pointer = value_type;
		using reference = value_type;
		using element_type = value_type;

		constexpr explicit Iterator() noexcept : x(0), step(0) {}
		constexpr explicit Iterator(T start, StepT step) noexcept : x(start), step(step) {}

		constexpr reference operator*() const noexcept { return x; }
		constexpr pointer operator->() const noexcept { return x; }

		constexpr Iterator& operator++() noexcept {
			x += step;
			return *this;
		}

		constexpr Iterator& operator--() noexcept {
			x -= step;
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
			return x == other.x;
		}

		constexpr auto operator<=>(const Iterator& other) const noexcept {
			return x <=> other.x;
		}

		constexpr bool operator==(Sentinel s) noexcept {
			return x >= s.end;
		}

		friend constexpr bool operator==(Sentinel s, const Iterator& iter) noexcept {
			return iter == s;
		}

	private:

		T x;
		StepT step;

	};


	using ReverseIterator = std::reverse_iterator<Iterator>;



	constexpr explicit Range(T stop) noexcept : Range(0, stop, 1) {}

	template<CC::Arithmetic T0, CC::Arithmetic T1, CC::Arithmetic T2, CC::Arithmetic U = TT::CommonType<T0, T1>>
	constexpr Range(T0 start, T1 stop, T2 step = 1) noexcept : start(start), stop(stop), step(step) {}


	constexpr Iterator begin() const noexcept {
		return Iterator(start, step);
	}

	constexpr Iterator cbegin() const noexcept {
		return begin();
	}

	constexpr Sentinel end() const noexcept {
		return Sentinel(stop);
	}

	constexpr Sentinel cend() const noexcept {
		return end();
	}

	constexpr ReverseIterator rbegin() const noexcept {
		return ReverseIterator(start, step);
	}

	constexpr ReverseIterator crbegin() const noexcept {
		return rbegin();
	}

	constexpr Sentinel rend() const noexcept {
		return end();
	}

	constexpr Sentinel crend() const noexcept {
		return end();
	}

private:

	T start;
	T stop;
	StepT step;

};



template<CC::Arithmetic T>
Range(T) -> Range<T, T>;

template<CC::Arithmetic T0, CC::Arithmetic T1, CC::Arithmetic T2>
Range(T0, T1, T2) -> Range<TT::CommonType<T0, T1>, T2>;