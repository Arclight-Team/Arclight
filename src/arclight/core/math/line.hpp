/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 line.hpp
 */

#pragma once

#include "vector.hpp"
#include "shapes/rectangle.hpp"
#include "common/concepts.hpp"
#include "arcconfig.hpp"

#include <optional>



template<CC::Float F>
class Line {

public:

	using Type = F;
	
	using VecT = Vec2<Type>;


	constexpr Line() noexcept : Line(VecT(), VecT()) {}

	template<CC::Arithmetic A, CC::Arithmetic B>
	constexpr Line(const Vec2<A>& start, const Vec2<B>& end) noexcept : start(start), end(end) {}


	__ARC_CMATH26 static Line fromAngle(const VecT& start, F angle, F length = {1}) {
		return {start, VecT(start.x + length * Math::cos(angle), start.y + length * Math::sin(angle))};
	}


	constexpr Rectangle<F> bounds() const noexcept {
		return {start, end - start};
	}


	constexpr VecT direction() const noexcept {
		return end - start;
	}

	constexpr F yOffset() const {
		return evaluate(0);
	}

	constexpr F root() const {
		return evaluateInverse(0);
	}

	constexpr F dx() const noexcept {
		return end.x - start.x;
	}

	constexpr F dy() const noexcept {
		return end.y - start.y;
	}

	__ARC_CMATH26 F angle() const {
		return Math::atan2(dy(), dx());
	}

	__ARC_CMATH26 F length() const {
		return Math::sqrt(dx() * dx() + dy() * dy());
	}


	constexpr void setPoints(const VecT& start, const VecT& end) noexcept {
		this->start = start;
		this->end = end;
	}
	
	
	constexpr F derivative() const noexcept {

		arc_assert(!Math::isZero(dx()), "Cannot obtain derivative of vertical line");

		return dy() / dx();

	}
	
	constexpr F evaluate(F x) const noexcept {
		return derivative() * (x - start.x) + start.y;
	}

	constexpr F evaluateInverse(F y) const noexcept {
		return (y - start.y) / derivative() + start.x;
	}
	

	constexpr bool contains(const VecT& point) const noexcept {
		return Math::equal(evaluateAt(point.x), point.y);
	}

	constexpr VecT closestPoint(const VecT& point) const noexcept {

		F c = end.x * start.y - start.x * end.y;
		F d = dx() * dx() + dy() * dy();

		return VecT((dx() * (dx() * point.x + dy() * point.y) - dy() * c) / d, (dy() * (dx() * point.x + dy() * point.y) + dx() * c) / d);

	}

	constexpr std::optional<VecT> intersection(const Line<F>& line) const noexcept {

		F d = line.dy() * dx() - dy() * line.dx();

		if (Math::isZero(d)) {
			return {};
		}

		F c0 = end.x * start.y - start.x * end.y;
		F c1 = line.end.x * line.start.y - line.start.x * line.end.y;

		return VecT((c0 * line.dx() - c1 * dx()) / d, (c0 * line.dy() - c1 * dy()) / d);

	}

	__ARC_CMATH26 F distance(const VecT& point) const {
		return Math::abs(dy() * point.x - dx() * point.y + end.x * start.y - start.x * end.y) / Math::sqrt(dy() * dy() + dx() * dx());
	}
	

	constexpr bool operator==(const Line<F>& line) const noexcept {
		return Math::equal(derivative(), line.derivative()) && Math::equal(yOffset(), line.yOffset());
	}


	VecT start, end;

};


template<CC::Float F>
RawLog& operator<<(RawLog& log, const Line<F>& line) {

	log << "Line[";
	log << "[" << line.start.x << ", " << line.start.y << "], ";
	log << "[" << line.end.x << ", " << line.end.y << "]]";

	return log;

}


ARC_DEFINE_FLOAT_ALIASES(1, Line, Line)
