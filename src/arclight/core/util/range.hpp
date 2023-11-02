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


template<SizeT N, CC::Arithmetic T, CC::Arithmetic StepT> requires(N != 0)
class Range {

public:

	static constexpr SizeT Dimensions = N;
	static constexpr bool Multidimensional = Dimensions > 1;


	class Iterator;

	struct Sentinel {

		constexpr explicit Sentinel(const std::array<T, N>& end) noexcept : end(end) {}

		std::array<T, N> end;

	};

	class Iterator {

	public:

		using iterator_category = std::bidirectional_iterator_tag;
		using value_type = TT::Conditional<Multidimensional, std::array<T, N>, T>;
		using pointer = value_type;
		using reference = value_type;
		using element_type = value_type;

		constexpr explicit Iterator() noexcept : cur(), step(0) {}
		constexpr explicit Iterator(const std::array<T, N>& start, StepT step) noexcept : start(start), cur(start), step(step) {}

		constexpr reference operator*() const noexcept {

			if constexpr (Multidimensional) {
				return cur;
			} else {
				return cur[0];
			}

		}

		constexpr pointer operator->() const noexcept {
			return operator*();
		}

		constexpr Iterator& operator++() noexcept {

			cur[0] += step;

			return *this;

		}

		constexpr Iterator& operator--() noexcept {

			cur[0] -= step;

			return *this;

		}

		constexpr Iterator operator++(int) noexcept {

			auto copy = *this;

			++(*this);

			return copy;

		}

		constexpr Iterator operator--(int) noexcept {

			auto copy = *this;

			--(*this);

			return copy;

		}

		constexpr bool operator==(const Iterator& other) const noexcept {

			for (SizeT i = 0; i < Dimensions; i++) {

				if (cur[i] != other.x[i]) {
					return false;
				}

			}

			return true;

		}

		constexpr auto operator<=>(const Iterator& other) const noexcept {
			return cur <=> other.cur;
		}

		constexpr bool operator==(Sentinel sentinel) noexcept {

			constexpr SizeT Count = Dimensions - 1;

			for (SizeT i = 0; i < Count; i++) {

				if (cur[i] >= sentinel.end[i]) {

					cur[i] = start[i];
					cur[i + 1] += step;

				}

			}

			return cur[Count] >= sentinel.end[Count];

		}

		friend constexpr bool operator==(Sentinel sentinel, const Iterator& iter) noexcept {
			return iter == sentinel;
		}

	private:

		std::array<T, N> start;
		std::array<T, N> cur;
		StepT step;

	};


	using ReverseIterator = std::reverse_iterator<Iterator>;


	constexpr Range(const std::array<T, N>& start, const std::array<T, N>& stop, StepT step = 1) noexcept requires(Multidimensional) : start(start), stop(stop), step(step) {}

	constexpr explicit Range(const std::array<T, N>& stop) noexcept requires(Multidimensional) : Range({}, stop, 1) {}

	constexpr Range(T start, T stop, StepT step = 1) noexcept requires(!Multidimensional) : start{start}, stop{stop}, step(step) {}

	constexpr explicit Range(T stop) noexcept requires(!Multidimensional) : Range(0, stop, 1) {}


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

	std::array<T, N> start;
	std::array<T, N> stop;
	StepT step;

};



template<SizeT N, CC::Arithmetic T> requires(N != 0)
Range(const T (&)[N]) -> Range<N, T, T>;

template<SizeT N, CC::Arithmetic T0, CC::Arithmetic T1, CC::Arithmetic T2> requires(N != 0)
Range(const T0 (&)[N], const T1 (&)[N], T2) -> Range<N, TT::CommonType<T0, T1>, T2>;

template<SizeT N, CC::Arithmetic T> requires(N != 0)
Range(const std::array<T, N>&) -> Range<N, T, T>;

template<SizeT N, CC::Arithmetic T0, CC::Arithmetic T1, CC::Arithmetic T2> requires(N != 0)
Range(const std::array<T0, N>&, const std::array<T1, N>&, T2) -> Range<N, TT::CommonType<T0, T1>, T2>;

template<CC::Arithmetic T>
Range(T) -> Range<1, T, T>;

template<CC::Arithmetic T0, CC::Arithmetic T1, CC::Arithmetic T2>
Range(T0, T1, T2) -> Range<1, TT::CommonType<T0, T1>, T2>;
