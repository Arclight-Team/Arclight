/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 line.hpp
 */

#pragma once

#include "vector.hpp"
#include "util/concepts.hpp"
#include "arcconfig.hpp"

#include <optional>



template<Float F>
class Line {

public:

	constexpr Line() : Line(Vec2<F>(), Vec2<F>()) {}

	template<Arithmetic A, Arithmetic B>
	constexpr Line(const Vec2<A>& start, const Vec2<B>& end) : start(start), end(end) {}


	constexpr Vec2<F> getStart() const {
		return start;
	}

	constexpr Vec2<F> getEnd() const {
		return end;
	}

	constexpr Vec2<F> getDirection() const {
		return end - start;
	}

	constexpr F angle() const {
		return Math::atan2(dy(), dx());
	}

	constexpr F yOffset() const {
		return evaluateAt(0);
	}

	constexpr F root() const {
		return evaluateInverse(0);
	}

	constexpr F dx() const {
		return end.x - start.x;
	}

	constexpr F dy() const {
		return end.y - start.y;
	}

	constexpr F length() const {
		return Math::sqrt(dx() * dx() + dy() * dy());
	}

	constexpr F derivative() const {
		arc_assert(!Math::isZero(dx()), "Cannot obtain derivative of vertical line");
		return dy() / dx();
	}

	constexpr F evaluateAt(F x) const {
		return derivative() * (x - start.x) + start.y;
	}

	constexpr F evaluateInverse(F y) const {
		return (y - start.y) / derivative() + start.x;
	}

	constexpr bool contains(const Vec2<F>& point) const {
		return Math::equal(evaluateAt(point.x), point.y);
	}

	constexpr F distance(const Vec2<F>& point) const {
		return Math::abs(dy() * point.x - dx() * point.y + end.x * start.y - start.x * end.y) / Math::sqrt(dy() * dy() + dx() * dx());
	}

	constexpr Vec2<F> closestPoint(const Vec2<F>& point) const {

		F c = end.x * start.y - start.x * end.y;
		F d = dx() * dx() + dy() * dy();

		return Vec2<F>((dx() * (dx() * point.x + dy() * point.y) - dy() * c) / d, (dy() * (dx() * point.x + dy() * point.y) + dx() * c) / d);

	}

	constexpr std::optional<Vec2<F>> intersection(const Line<F>& line) const {

		F d = line.dy() * dx() - dy() * line.dx();

		if(Math::isZero(d)) {
			return {};
		}

		F c0 = end.x * start.y - start.x * end.y;
		F c1 = line.end.x * line.start.y - line.start.x * line.end.y;

		return Vec2<F>((c0 * line.dx() - c1 * dx()) / d, (c0 * line.dy() - c1 * dy()) / d);

	}

	constexpr bool equal(const Line<F>& line) const {
		return Math::equal(derivative(), line.derivative()) && Math::equal(yOffset(), line.yOffset());
	}

	constexpr bool operator=(const Line<F>& line) const {
		return equal(line);
	}


	constexpr void setPoints(const Vec2<F>& start, const Vec2<F>& end) {
		this->start = start;
		this->end = end;
	}
	

	constexpr static Line<F> fromAngle(const Vec2<F>& start, F angle, F length = F(1)) {
		return Line<F>(start, Vec2<F>(start.x + length * Math::cos(angle), start.y + length * Math::sin(angle)));
	}


	Vec2<F> start, end;

};



using LineF     = Line<float>;
using LineD     = Line<double>;
using LineLD    = Line<long double>;
using LineX     = Line<ARC_STD_FLOAT_TYPE>;