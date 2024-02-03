/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 bezier.hpp
 */

#pragma once

#include "math.hpp"
#include "vector.hpp"
#include "shapes/rectangle.hpp"
#include "line.hpp"

#include <array>
#include <span>


template<u32 D, CC::Float F>
class Bezier {

public:

	using Type = F;

	static constexpr u32 Order = D;

	static constexpr bool Linear = Order == 1;

	static_assert(Order, "Order must be at least 1");


	constexpr Bezier() noexcept : Bezier(Vec2<F>(0, 0)) {}

	template<CC::Vector... V>
	constexpr explicit Bezier(const V&... cps) noexcept : controlPoints {cps...} {}

	template<CC::Vector V>
	constexpr explicit Bezier(const std::span<V>& cps) {

		std::copy(cps.begin(), cps.end(), controlPoints);
		std::fill_n(&controlPoints[cps.size()], Math::max<i32>(Order + 1 - cps.size(), 0), Vec2<F>(0, 0));

	}


	constexpr Vec2<F> evaluate(double t) const {

		if constexpr (Linear) {
			return Math::lerp(controlPoints[0], controlPoints[1], t);
		} else {
			return evaluateHelper(t, std::make_index_sequence<Order>{});
		}

	}


	constexpr Bezier<Order - 1, F> derivative() const {

		static_assert(Order >= 2, "Derivative is not a bezier curve");

		std::array<Vec2<F>, Order> a;

		for(u32 i = 0; i < Order; i++) {
			a[i] = Order * (controlPoints[i + 1] - controlPoints[i]);
		}

		return Bezier<Order - 1, F>(std::span{a.data(), a.size()});

	}

	constexpr Bezier<Order - 2, F> secondDerivative() const {

		static_assert(Order >= 3, "Second derivative is not a bezier curve");

		std::array<Vec2<F>, Order - 1> a;

		Vec2<F> first = Order * (controlPoints[1] - controlPoints[0]);

		for(u32 i = 0; i < Order; i++) {

			Vec2<F> second = Order * (controlPoints[i + 2] - controlPoints[i + 1]);
			a[i] = (Order - 1) * (second - first);
			first = second;

		}

		return Bezier<Order - 2, F>(std::span{a.data(), a.size()});

	}


	constexpr Rectangle<F> bounds() const {

		if constexpr (Linear) {

			// Trivial case, simply enclose line by rect
			return Rectangle<F>::fromPoints(controlPoints[0], controlPoints[1]);

		} else if constexpr (Order <= 3) {

			Bezier<Order - 1, F> drv = derivative();

			F lx = getStartPoint().x;
			F hx = getEndPoint().x;
			F ly = getStartPoint().y;
			F hy = getEndPoint().y;

			Math::ascend(lx, hx);
			Math::ascend(ly, hy);

			if constexpr (Order == 2) {
				
				// Linear derivative
				const Vec2<F>& d0 = drv.getStartPoint();
				const Vec2<F>& d1 = drv.getEndPoint();

				if (!Math::equal(d1.x, d0.x)) {

					F t = -d0.x / (d1.x - d0.x);

					if (Math::inRange(t, 0, 1)) {

						Vec2<F> p = evaluate(t);
						lx = Math::min(lx, p.x);
						hx = Math::max(hx, p.x);

					}

				}

				if (!Math::equal(d1.y, d0.y)) {

					F t = -d0.y / (d1.y - d0.y);
									
					if (Math::inRange(t, 0, 1)) {

						Vec2<F> p = evaluate(t);
						ly = Math::min(ly, p.y);
						hy = Math::max(hy, p.y);

					}

				}

			} else if constexpr (Order == 3) {

				// Quadratic derivative
				const Vec2<F>& d0 = drv.getStartPoint();
				const Vec2<F>& d1 = drv.template getControlPoint<1>();
				const Vec2<F>& d2 = drv.getEndPoint();

				Vec2<F> a = d0 - 2 * d1 + d2;
				Vec2<F> b = 2 * (d1 - d0);
				Vec2<F> c = d0;

				Vec2<F> d = b * b - 4 * a * c;

				if (d.x > 0) {

					F t0 = (-b.x + Math::sqrt(d.x)) / (2 * a.x);
					F t1 = (-b.x - Math::sqrt(d.x)) / (2 * a.x);

					if (Math::inRange(t0, 0, 1)) {

						Vec2<F> p = evaluate(t0);
						lx = Math::min(lx, p.x);
						hx = Math::max(hx, p.x);

					}

					if (Math::inRange(t1, 0, 1)) {

						Vec2<F> p = evaluate(t1);
						lx = Math::min(lx, p.x);
						hx = Math::max(hx, p.x);

					}

				} else if (Math::isZero(d.x)) {

					F t = -b.x / (2 * a.x);

					if (Math::inRange(t, 0, 1)) {

						Vec2<F> p = evaluate(t);
						lx = Math::min(lx, p.x);
						hx = Math::max(hx, p.x);

					}

				}

				if (d.y > 0) {

					F t0 = (-b.y + Math::sqrt(d.y)) / (2 * a.y);
					F t1 = (-b.y - Math::sqrt(d.y)) / (2 * a.y);

					if (Math::inRange(t0, 0, 1)) {

						Vec2<F> p = evaluate(t0);
						ly = Math::min(ly, p.y);
						hy = Math::max(hy, p.y);

					}

					if (Math::inRange(t1, 0, 1)) {

						Vec2<F> p = evaluate(t1);
						ly = Math::min(ly, p.y);
						hy = Math::max(hy, p.y);

					}

				} else if (Math::isZero(d.y)) {

					F t = -b.y / (2 * a.y);

					if (Math::inRange(t, 0, 1)) {

						Vec2<F> p = evaluate(t);
						ly = Math::min(ly, p.y);
						hy = Math::max(hy, p.y);

					}

				}
				
			}

			return Rectangle<F>::fromPoints(Vec2<F>(lx, ly), Vec2<F>(hx, hy));

		} else {

			static_assert("Bezier bounding boxes above degree 5 are unsupported");

		}

	}

	constexpr std::array<std::optional<F>, D> parameterFromX(F x) {
		return getParameter<false>(x);
	}

	constexpr std::array<std::optional<F>, D> parameterFromY(F y) {
		return getParameter<true>(y);
	}

	constexpr std::array<std::optional<F>, D> getX(F y) {
		return getComponent<true>(y);
	}

	constexpr std::array<std::optional<F>, D> getY(F x) {
		return getComponent<false>(x);
	}


	constexpr Vec2<F> getStartPoint() const {
		return getControlPoint<0>();
	}

	constexpr Vec2<F> getEndPoint() const {
		return getControlPoint<D>();
	}

	template<u32 I>
	constexpr Vec2<F> getControlPoint() const requires (I < D + 1) {
		return controlPoints[I];
	}

	constexpr Vec2<F> getControlPoint(u32 i) const {
		return controlPoints[i];
	}


	constexpr void setStartPoint(const Vec2<F>& start) {
		setControlPoint<0>(start);
	}

	constexpr void setEndPoint(const Vec2<F>& end) {
		setControlPoint<D>(end);
	}

	template<u32 I>
	constexpr void setControlPoint(const Vec2<F>& point) requires (I < D + 1) {
		this->controlPoints[I] = point;
	}

	constexpr void setControlPoint(u32 i, const Vec2<F>& point) {
		this->controlPoints[i] = point;
	}


	Vec2<F> controlPoints[D + 1];

private:

	//B = false: x -> y, B = true: y -> x
	template<bool B>
	constexpr std::array<std::optional<F>, D> getComponent(F v) {

		auto a = getParameter<B>(v);

		for (u32 i = 0; i < D; i++) {

			if (!a[i]) {
				break;
			}

			a[i] = evaluate(a[i])[!B];

		}

		return a;

	}

	//B = false: x -> t, B = true: y -> t
	template<bool B>
	constexpr std::array<std::optional<F>, Order> getParameter(F v) {

		if constexpr (Linear) {

			// Linear bezier
			F a = getStartPoint()[B];
			F b = getEndPoint()[B];

			F m = b - a;
			
			// If m is also 0, there is no solution
			if (Math::isZero(m)) [[unlikely]] {
				return {};
			}

			F t = (v - a) / m;

			if (Math::inRange(t, 0, 1)) {
				return {t};
			} else {
				return {};
			}

		} else if constexpr (Order == 2) {

			// Quadratic bezier
			F a = getStartPoint()[B];
			F b = getControlPoint<1>()[B];
			F c = getEndPoint()[B];

			F n = a - 2 * b + c;
			F d = b * b - a * c + v * n;

			// If n is 0, the quadratic bezier decays to a linear problem
			if (Math::isZero(n)) [[unlikely]] {

				F m = b - a;

				// If m is also 0, there is no solution
				if (Math::isZero(m)) [[unlikely]] {
					return {};
				}

				F t = (v - a) / (2 * m);

				// If t is in range, it's the only solution
				if (!Math::inRange(t, 0, 1)) {
					return {};
				}

				return {t};
			
			// No t for which the component function passes x/y
			} else if (d < 0) [[unlikely]] {

				return {};

			// One t for which the component function passes x/y
			} else if (Math::isZero(d)) [[unlikely]] {

				F t = (a - b) / n;

				if (!Math::inRange(t, 0, 1)) {
					return {};
				}
				
				return {t};

			// Two t for which the component function passes x/y
			} else [[likely]] {

				Vec2<F> t = {
					(a - b - Math::sqrt(d)) / n,
					(a - b + Math::sqrt(d)) / n
				};

				bool t0Valid = Math::inRange(t[0], 0, 1);
				bool t1Valid = Math::inRange(t[1], 0, 1);

				u32 solutions = t0Valid + t1Valid;

				switch (solutions) {

					case 0:
					default:
						return {};

					case 1:

						if (t0Valid) {
							return {t[0]};
						} else {
							return {t[1]};
						}

					case 2:

						Math::ascend(t[0], t[1]);
						return {t[0], t[1]};

				}

			}

		} else {

			static_assert("Bezier coordinate solutions above degree 2 are unsupported");

		}

	}

	template<SizeT... Pack>
	constexpr Vec2<F> evaluateHelper(double t, std::index_sequence<Pack...>) const requires (Order <= 32) {

		std::array<Vec2<F>, sizeof...(Pack)> c;

		for (SizeT i = 0; i < c.size(); i++) {
			c[i] = Math::lerp(controlPoints[i], controlPoints[i + 1], t);
		}

		return Bezier<Order - 1, F>(std::span{c.data(), c.size()}).evaluate(t);

	}

};


template<u32 D, CC::Float F>
RawLog& operator<<(RawLog& log, const Bezier<D, F>& bezier) {

	log << "Bezier[";

	for (u32 i = 0; const Vec2<F>& point : bezier.controlPoints) {

		log << "[" << point.x << ", " << point.y;

		if (i++ == bezier.Order) {
			log << "]]";
		} else {
			log << "], ";
		}

	}

	return log;

}


ARC_DEFINE_FLOAT_ALIASES(0, Bezier, Bezier1, 1)
ARC_DEFINE_FLOAT_ALIASES(0, Bezier, Bezier2, 2)
ARC_DEFINE_FLOAT_ALIASES(0, Bezier, Bezier3, 3)
ARC_DEFINE_FLOAT_ALIASES(0, Bezier, Bezier4, 4)
ARC_DEFINE_FLOAT_ALIASES(0, Bezier, Bezier5, 5)
