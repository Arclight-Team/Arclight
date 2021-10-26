#pragma once

#include "math.h"
#include "vector.h"
#include <array>
#include <vector>
#include <span>


template<u32 Degree, Float F>
class Bezier {

public:

    static_assert(Degree, "Degree must be at least 1");

    using Type = F;
    constexpr static u32 Order = Degree;


    constexpr Bezier() : Bezier(Vec2<F>(0, 0)) {}

    template<Vector... V>
    constexpr Bezier(const V&... cps) : controlPoints {cps...} {}

    template<Vector V>
    constexpr Bezier(const std::span<V>& cps) {
        std::copy(cps.begin(), cps.end(), controlPoints);
        std::fill_n(&controlPoints[cps.size()], Math::max<i32>(Degree + 1 - cps.size(), 0), Vec2<F>(0, 0));
    }


    constexpr Vec2<F> evaluate(double t) const {

        if constexpr (Degree == 1) {
            return Math::lerp(controlPoints[0], controlPoints[1], t);
        } else {
            return evaluateHelper(t, std::make_index_sequence<Degree>{});
        }

    }


    constexpr Vec2<F> getStartPoint() const {
        return getControlPoint<0>();
    }

    constexpr Vec2<F> getEndPoint() const {
        return getControlPoint<Degree>();
    }

    template<u32 I>
    constexpr Vec2<F> getControlPoint() const requires (I < Degree + 1) {
        return controlPoints[I];
    }

    constexpr Vec2<F> getControlPoint(u32 i) const {
        return controlPoints[i];
    }


    constexpr void setStartPoint(const Vec2<F>& start) {
        setControlPoint<0>(start);
    }

    constexpr void setEndPoint(const Vec2<F>& end) {
        setControlPoint<Degree>(end);
    }

    template<u32 I>
    constexpr void setControlPoint(const Vec2<F>& point) requires (I < Degree + 1) {
        this->controlPoints[I] = point;
    }

    constexpr void setControlPoint(u32 i, const Vec2<F>& point) {
        this->controlPoints[i] = point;
    }


    Vec2<F> controlPoints[Degree + 1];

private:

    template<SizeT... Pack>
    constexpr auto evaluateHelper(double t, std::index_sequence<Pack...>) const {

        constexpr static bool useHeap = Degree > 128;
        constexpr static SizeT Count = sizeof...(Pack);
        using Container = std::conditional_t<useHeap, std::vector<Vec2<F>>, std::array<Vec2<F>, Count>>;

        Container c;

        if constexpr (useHeap) {
            c.resize(Count);
        }

        for(SizeT i = 0; i < c.size(); i++) {
            c[i] = Math::lerp(controlPoints[i], controlPoints[i + 1], t);
        }

        return Bezier<Degree - 1, F>(std::span{c.data(), c.size()}).evaluate(t);

    }

};



#define BEZIER_DEFINE_NDTS(name, degree, type, suffix) typedef Bezier<degree, type> name##degree##suffix;

#define BEZIER_DEFINE_ND(name, degree) \
	BEZIER_DEFINE_NDTS(name, degree, float, f) \
	BEZIER_DEFINE_NDTS(name, degree, double, d) \
	BEZIER_DEFINE_NDTS(name, degree, long double, ld) \
	BEZIER_DEFINE_NDTS(name, degree, ARC_STD_FLOAT_TYPE, x)

#define BEZIER_DEFINE_N(name) \
    BEZIER_DEFINE_ND(name, 1) \
    BEZIER_DEFINE_ND(name, 2) \
    BEZIER_DEFINE_ND(name, 3) \
    BEZIER_DEFINE_ND(name, 4) \
    BEZIER_DEFINE_ND(name, 5)

#define BEZIER_DEFINE \
	BEZIER_DEFINE_N(Bezier) \
	BEZIER_DEFINE_N(BezierCurve)

BEZIER_DEFINE

#undef BEZIER_DEFINE
#undef BEZIER_DEFINE_N
#undef BEZIER_DEFINE_ND
#undef BEZIER_DEFINE_NDTS
