/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 rectangle.hpp
 */

#pragma once

#include "shape.hpp"
#include "../vector.hpp"



template<CC::Arithmetic A>
class Rectangle : public IShape {

public:

	using Type = A;

	using ScalarT = TT::ToFloat<Type>;
	using VecT = Vec2<Type>;


	constexpr Rectangle() noexcept : Rectangle(0, 0, 0, 0) {}

	constexpr Rectangle(A x, A y, A w, A h) : x(x), y(y), w(w), h(h) {}

	constexpr Rectangle(const VecT& pos, const VecT& size) noexcept : Rectangle(pos.x, pos.y, size.x, size.y) {}

	template<CC::Arithmetic B>
	constexpr Rectangle(const Rectangle<B>& rect) noexcept : Rectangle(rect.x, rect.y, rect.w, rect.h) {}


	constexpr static Rectangle fromPoints(VecT start, VecT end) noexcept {

		Math::ascend(start.x, end.x);
		Math::ascend(start.y, end.y);

		return Rectangle(start, end - start);

	}
	

	constexpr auto toIntegerRect() const requires (CC::Float<A>) {

		using I = TT::ToInteger<A>;

		I nx = I(Math::floor(x));
		I ny = I(Math::floor(y));
		I nw = I(Math::ceil(end().x)) - nx;
		I nh = I(Math::ceil(end().y)) - ny;

		return Rectangle<I>(nx, ny, nw, nh);

	}


	constexpr bool valid() noexcept {
		return Math::greater(w, 0) && Math::greater(h, 0);
	}

	constexpr VecT start() const noexcept {
		return {x, y};
	}

	constexpr VecT end() const noexcept {
		return {x + w, x + h};
	}

	constexpr Rectangle bounds() const noexcept {
		return *this;
	}


	constexpr A area() const noexcept {
		return w * h;
	}

	constexpr A perimeter() const noexcept {
		return (w + h) * 2;
	}

	constexpr VecT size() const noexcept {
		return {w, h};
	}

	constexpr A diagonalSquared() const noexcept {
		return w * w + h * h;
	}

	__ARC_CMATH26 ScalarT diagonal() const {
		return Math::sqrt<ScalarT>(diagonalSquared());
	}


	constexpr void setPosition(const VecT& pos) noexcept {
		x = pos.x;
		y = pos.y;
	}

	constexpr void setSize(const VecT& size) noexcept {
		w = size.x;
		h = size.y;
	}


	template<CC::Arithmetic B>
	constexpr bool intersects(const Rectangle<B>& other) const {

		VecT s1 = start();
		VecT e1 = end();
		Vec2<B> s2 = other.start();
		Vec2<B> e2 = other.end();

		return
			s2.x < e1.x && e2.x > s1.x &&
			s2.y < e1.y && e2.y > s1.y;

	}

	template<CC::Arithmetic B>
	constexpr bool contains(const Rectangle<B>& other) const {

		VecT s1 = start();
		VecT e1 = end();
		Vec2<B> s2 = other.getPosition();
		Vec2<B> e2 = other.getEndpoint();

		return
				s2.x > s1.x && e1.x > e2.x &&
				s2.y > s1.y && e1.y > e2.y;

	}

	template<CC::Arithmetic B>
	constexpr bool contains(const Vec2<B>& point) const {
		return point.x >= x && point.x <= end().x && point.y >= y && point.y <= end().y;
	}


	template<CC::Arithmetic B>
	constexpr Rectangle<A> intersect(const Rectangle<B>& other) const {

		A ax0 = x;
		A ax1 = x + w;
		A bx0 = other.x;
		A bx1 = other.x + other.w;
		A ay0 = y;
		A ay1 = y + h;
		A by0 = other.y;
		A by1 = other.y + other.h;

		if (ax0 > bx0) {
			std::swap(ax0, bx0);
			std::swap(ax1, bx1);
		}

		if (ay0 > by0) {
			std::swap(ay0, by0);
			std::swap(ay1, by1);
		}

		Rectangle<A> r;

		// First x
		if (ax1 <= bx0) {

			// No intersection
			return {};

		} else if (ax1 >= bx1) {

			// Full containment
			r.x = bx0;
			r.w = bx1 - bx0;

		} else {

			// Partial overlap
			r.x = bx0;
			r.w = ax1 - ax0 + bx1 - bx0 - (bx1 - ax0);

		}

		// Then y
		if (ay1 <= by0) {

			// No intersection
			return {};

		} else if (ay1 >= by1) {

			// Full containment
			r.y = by0;
			r.h = by1 - by0;

		} else {

			// Partial overlap
			r.y = by0;
			r.h = ay1 - ay0 + by1 - by0 - (by1 - ay0);

		}

		return r;

	}

	template<CC::Arithmetic B>
	constexpr Rectangle<A> unite(const Rectangle<B>& other) const {
		return Rectangle<A>::fromPoints(VecT(Math::min(x, other.x), Math::min(y, other.y)), VecT(Math::max(x, other.x), Math::max(y, other.y)));
	}


	template<CC::Arithmetic B>
	constexpr bool operator==(const Rectangle<B>& other) const {
		return Math::equal(x, other.x) && Math::equal(y, other.y) && Math::equal(w, other.w) && Math::equal(h, other.h);
	}

	template<CC::Arithmetic B>
	constexpr Rectangle<A>& operator|=(const Rectangle<B>& other) const {

		*this = unite(other);
		return *this;

	}

	template<CC::Arithmetic B>
	constexpr Rectangle<A>& operator&=(const Rectangle<B>& other) const {

		*this = intersect(other);
		return *this;

	}


	A x;
	A y;
	A w;
	A h;

};


template<CC::Arithmetic A, CC::Arithmetic B>
constexpr Rectangle<A> operator|(const Rectangle<A>& a, const Rectangle<B>& b) {
	return a.unite(b);
}

template<CC::Arithmetic A, CC::Arithmetic B>
constexpr Rectangle<A> operator&(const Rectangle<A>& a, const Rectangle<B>& b) {
	return a.intersect(b);
}


template<CC::Arithmetic A>
RawLog& operator<<(RawLog& log, const Rectangle<A>& rect) {

	log << "Rect[";
	log << rect.x << ", ";
	log << rect.y << ", ";
	log << rect.w << ", ";
	log << rect.h << "]";

	return log;

}


ARC_DEFINE_ARITHMETIC_ALIASES(1, Rectangle, Rect)
