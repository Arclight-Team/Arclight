/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 rectangle.hpp
 */

#pragma once

#include "vector.hpp"
#include "common/concepts.hpp"
#include "arcconfig.hpp"


template<CC::Arithmetic A>
class Rectangle {

public:

	using Type = A;

	constexpr Rectangle() : Rectangle(0, 0, 0, 0) {}
	constexpr Rectangle(const Vec2<A>& pos, const Vec2<A>& size) : Rectangle(pos.x, pos.y, size.x, size.y) {}

	constexpr Rectangle(A x, A y, A w, A h) : x(x), y(y), w(w), h(h) {}

	template<CC::Arithmetic B>
	constexpr Rectangle(const Rectangle<B>& rect) : Rectangle(rect.x, rect.y, rect.w, rect.h) {}

	template<CC::Arithmetic B>
	constexpr Rectangle<A>& operator=(const Rectangle<B>& other) {

		x = other.x;
		y = other.y;
		w = other.w;
		h = other.h;
		return *this;

	}

	constexpr auto toIntegerRect() const requires (CC::Float<A>) {

		using Int = TT::ToInteger<A>;

		Int nx = static_cast<Int>(Math::floor(x));
		Int ny = static_cast<Int>(Math::floor(y));
		Int nw = static_cast<Int>(Math::ceil(getEndX())) - nx;
		Int nh = static_cast<Int>(Math::ceil(getEndY())) - ny;

		return Rectangle<Int>(nx, ny, nw, nh);

	}

	constexpr A getX() const {
		return x;
	}

	constexpr A getY() const {
		return y;
	}

	constexpr A getWidth() const {
		return w;
	}

	constexpr A getHeight() const {
		return h;
	}

	constexpr A getEndX() const {
		return x + w;
	}

	constexpr A getEndY() const {
		return y + h;
	}

	constexpr void setX(A x) {
		this->x = x;
	}

	constexpr void setY(A y) {
		this->y = y;
	}

	constexpr void setWidth(A w) {
		this->w = w;
	}

	constexpr void setHeight(A h) {
		this->h = h;
	}


	constexpr Vec2<A> getPosition() const {
		return Vec2<A>(x, y);
	}

	constexpr Vec2<A> getSize() const {
		return Vec2<A>(w, h);
	}

	constexpr Vec2<A> getEndpoint() const {
		return Vec2<A>(x + w, y + h);
	}

	constexpr void setPosition(const Vec2<A>& pos) {
		setPosition(pos.x, pos.y);
	}

	constexpr void setPosition(A x, A y) {
		this->x = x;
		this->y = y;
	}

	constexpr void setSize(const Vec2<A>& size) {
		setSize(size.x, size.y);
	}

	constexpr void setSize(A x, A y) {
		this->w = w;
		this->h = h;
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

		if(ax0 > bx0) {
			std::swap(ax0, bx0);
			std::swap(ax1, bx1);
		}

		if(ay0 > by0) {
			std::swap(ay0, by0);
			std::swap(ay1, by1);
		}

		Rectangle<A> r;

		//First x
		if(ax1 <= bx0) {

			//No intersection
			return Rectangle<A>();

		} else if(ax1 >= bx1) {

			//Full containment
			r.x = bx0;
			r.w = bx1 - bx0;

		} else {

			//Partial overlap
			r.x = bx0;
			r.w = ax1 - ax0 + bx1 - bx0 - (bx1 - ax0);

		}

		//Then y
		if(ay1 <= by0) {

			//No intersection
			return Rectangle<A>();

		} else if(ay1 >= by1) {

			//Full containment
			r.y = by0;
			r.h = by1 - by0;

		} else {

			//Partial overlap
			r.y = by0;
			r.h = ay1 - ay0 + by1 - by0 - (by1 - ay0);

		}

		return r;

	}

	template<CC::Arithmetic B>
	constexpr bool intersects(const Rectangle<B>& other) const {

		Vec2<A> s1 = getPosition();
		Vec2<A> e1 = getEndpoint();
		Vec2<B> s2 = other.getPosition();
		Vec2<B> e2 = other.getEndpoint();

		return
			s2.x < e1.x && e2.x > s1.x &&
			s2.y < e1.y && e2.y > s1.y;

	}

	constexpr A perimeter() const {
		return (w + h) * 2;
	}

	constexpr A area() const {
		return w * h;
	}

	constexpr bool contains(const Vec2<A>& point) const {
		return point.x >= x && point.x <= getEndX() && point.y >= y && point.y <= getEndY();
	}

	// Checks if the rectangle contains another rectangle
	template<CC::Arithmetic B>
	constexpr bool contains(const Rectangle<B>& other) const {
		
		Vec2<A> s1 = getPosition();
		Vec2<A> e1 = getEndpoint();
		Vec2<B> s2 = other.getPosition();
		Vec2<B> e2 = other.getEndpoint();

		return
			s2.x > s1.x && e1.x > e2.x &&
			s2.y > s1.y && e1.y > e2.y;

	}

	template<CC::Arithmetic B>
	constexpr Rectangle<A> unite(const Rectangle<B>& other) const {
		return Rectangle<A>::fromPoints(Vec2<A>(Math::min(x, other.x), Math::min(y, other.y)), Vec2<A>(Math::max(x, other.x), Math::max(y, other.y)));
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

	constexpr static Rectangle<A> fromPoints(Vec2<A> start, Vec2<A> end) {

		Math::ascOrder(start.x, end.x);
		Math::ascOrder(start.y, end.y);

		return Rectangle<A>(start, end - start);

	}


	A x, y, w, h;

};


template<CC::Arithmetic A, CC::Arithmetic B>
constexpr auto operator|(const Rectangle<A>& a, const Rectangle<B>& b) {
	return a.unite(b);
}

template<CC::Arithmetic A, CC::Arithmetic B>
constexpr auto operator&(const Rectangle<A>& a, const Rectangle<B>& b) {
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


using RectB     = Rectangle<bool>;
using RectC     = Rectangle<i8>;
using RectS     = Rectangle<i16>;
using RectI     = Rectangle<i32>;
using RectL     = Rectangle<i64>;
using RectUC    = Rectangle<u8>;
using RectUS    = Rectangle<u16>;
using RectUI    = Rectangle<u32>;
using RectUL    = Rectangle<u64>;
using RectF     = Rectangle<float>;
using RectD     = Rectangle<double>;
using RectLD    = Rectangle<long double>;
using RectX     = Rectangle<ARC_STD_FLOAT_TYPE>;
using Rect      = RectX;