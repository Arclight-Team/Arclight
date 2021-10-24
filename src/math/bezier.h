#pragma once

#include "math.h"
#include "vector.h"


template<u32 Degree, Float F>
class Bezier {

public:

    static_assert(Degree, "Degree must be at least 1");


    constexpr Bezier() : Bezier(Vec2<F>(0, 0), Vec2<F>(0, 0)) {}

    template<Vector... V>
    constexpr Bezier(const Vec2<F>& start, const Vec2<F>& end, const V&... cps) : start(start), end(end), controlPoints {cps...} {}


    Vec2<F> start;
    Vec2<F> end;
    Vec2<F> controlPoints[Degree - 1];

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
