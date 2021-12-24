#pragma once

#include "math.hpp"
#include "vector.hpp"
#include "rectangle.hpp"
#include "line.hpp"

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


    constexpr Bezier<Degree - 1, F> derivative() const {

        static_assert(Degree >= 2, "Derivative is not a bezier curve");

        std::array<Vec2<F>, Degree> a;

        for(u32 i = 0; i < Degree; i++) {
            a[i] = Degree * (controlPoints[i + 1] - controlPoints[i]);
        }

        return Bezier<Degree - 1, F>(std::span{a.data(), a.size()});

    }

    constexpr Bezier<Degree - 2, F> secondDerivative() const {

        static_assert(Degree >= 3, "Second derivative is not a bezier curve");

        std::array<Vec2<F>, Degree - 1> a;

        Vec2<F> first = Degree * (controlPoints[1] - controlPoints[0]);

        for(u32 i = 0; i < Degree; i++) {

            Vec2<F> second = Degree * (controlPoints[i + 2] - controlPoints[i + 1]);
            a[i] = (Degree - 1) * (second - first);
            first = second;

        }

        return Bezier<Degree - 2, F>(std::span{a.data(), a.size()});

    }


    constexpr Rectangle<F> boundingBox() const {

        if constexpr (Degree == 1) {

            //Trivial case, simply enclose line by rect
            return Rectangle<F>::fromPoints(controlPoints[0], controlPoints[1]);

        } else if constexpr (Degree <= 3) {

            Bezier<Degree - 1, F> drv = derivative();

            F lx = getStartPoint().x;
            F hx = getEndPoint().x;
            F ly = getStartPoint().y;
            F hy = getEndPoint().y;

            Math::ascOrder(lx, hx);
            Math::ascOrder(ly, hy);

            if constexpr (Degree == 2) {
                
                //Linear derivative
                const Vec2<F>& d0 = drv.getStartPoint();
                const Vec2<F>& d1 = drv.getEndPoint();

                if(!Math::equal(d1.x, d0.x)) {

                    F t = -d0.x / (d1.x - d0.x);

                    if(Math::inRange(t, 0, 1)) {

                        Vec2<F> p = evaluate(t);
                        lx = Math::min(lx, p.x);
                        hx = Math::max(hx, p.x);

                    }

                }

                if(!Math::equal(d1.y, d0.y)) {

                    F t = -d0.y / (d1.y - d0.y);
                                    
                    if(Math::inRange(t, 0, 1)) {

                        Vec2<F> p = evaluate(t);
                        ly = Math::min(ly, p.y);
                        hy = Math::max(hy, p.y);

                    }

                }

            } else if constexpr (Degree == 3) {

                //Quadratic derivative
                const Vec2<F>& d0 = drv.getStartPoint();
                const Vec2<F>& d1 = drv.getControlPoint<1>();
                const Vec2<F>& d2 = drv.getEndPoint();

                Vec2<F> a = d0 - 2 * d1 + d2;
                Vec2<F> b = 2 * (d1 - d0);
                Vec2<F> c = d0;

                Vec2<F> d = b * b - 4 * a * c;

                if(d.x > 0) {

                    F t0 = (-b.x + Math::sqrt(d.x)) / (2 * a.x);
                    F t1 = (-b.x - Math::sqrt(d.x)) / (2 * a.x);

                    if(Math::inRange(t0, 0, 1)) {

                        Vec2<F> p = evaluate(t0);
                        lx = Math::min(lx, p.x);
                        hx = Math::max(hx, p.x);

                    }

                    if(Math::inRange(t1, 0, 1)) {

                        Vec2<F> p = evaluate(t1);
                        lx = Math::min(lx, p.x);
                        hx = Math::max(hx, p.x);

                    }

                } else if (Math::isZero(d.x)) {

                    F t = -b.x / (2 * a.x);

                    if(Math::inRange(t, 0, 1)) {

                        Vec2<F> p = evaluate(t);
                        lx = Math::min(lx, p.x);
                        hx = Math::max(hx, p.x);

                    }

                }

                if(d.y > 0) {

                    F t0 = (-b.y + Math::sqrt(d.y)) / (2 * a.y);
                    F t1 = (-b.y - Math::sqrt(d.y)) / (2 * a.y);

                    if(Math::inRange(t0, 0, 1)) {

                        Vec2<F> p = evaluate(t0);
                        ly = Math::min(ly, p.y);
                        hy = Math::max(hy, p.y);

                    }

                    if(Math::inRange(t1, 0, 1)) {

                        Vec2<F> p = evaluate(t1);
                        ly = Math::min(ly, p.y);
                        hy = Math::max(hy, p.y);

                    }

                } else if (Math::isZero(d.y)) {

                    F t = -b.y / (2 * a.y);

                    if(Math::inRange(t, 0, 1)) {

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

    constexpr std::array<std::optional<F>, Degree> parameterFromX(F x) {
        return getParameter<false>(x);
    }

    constexpr std::array<std::optional<F>, Degree> parameterFromY(F y) {
        return getParameter<true>(y);
    }

    constexpr std::array<std::optional<F>, Degree> getX(F y) {
        return getComponent<true>(y);
    }

    constexpr std::array<std::optional<F>, Degree> getY(F x) {
        return getComponent<false>(x);
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

    //B = false: x -> y, B = true: y -> x
    template<bool B>
    constexpr std::array<std::optional<F>, Degree> getComponent(F v) {

        auto a = getParameter<B>(v);

        for(u32 i = 0; i < Degree; i++) {

            if(!a[i]) {
                break;
            }

            a[i] = evaluate(a[i])[!B];

        }

        return a;

    }

    //B = false: x -> t, B = true: y -> t
    template<bool B>
    constexpr std::array<std::optional<F>, Degree> getParameter(F v) {

        if constexpr (Degree == 1) {

            //Linear bezier
            F a = getStartPoint()[B];
            F b = getEndPoint()[B];

            F m = b - a;
            
            //If m is also 0, there is no solution
            if(Math::isZero(m)) [[unlikely]] {
                return {};
            }

            F t = (v - a) / m;

            if(Math::inRange(t, 0, 1)) {
                return {t};
            } else {
                return {};
            }

        } else if constexpr (Degree == 2) {

            //Quadratic bezier
            F a = getStartPoint()[B];
            F b = getControlPoint<1>()[B];
            F c = getEndPoint()[B];

            F n = a - 2 * b + c;
            F d = b * b - a * c + v * n;

            //If n is 0, the quadratic bezier decays to a linear problem
            if(Math::isZero(n)) [[unlikely]] {

                F m = b - a;

                //If m is also 0, there is no solution
                if(Math::isZero(m)) [[unlikely]] {
                    return {};
                }

                F t = (v - a) / (2 * m);

                //If t is in range, it's the only solution
                if(!Math::inRange(t, 0, 1)) {
                    return {};
                }

                return {t};
            
            //No t for which the component function passes x/y
            } else if(d < 0) [[unlikely]] {

                return {};

            //One t for which the component function passes x/y
            } else if (Math::isZero(d)) [[unlikely]] {

                F t = (a - b) / n;

                if(!Math::inRange(t, 0, 1)) {
                    return {};
                }
                
                return {t};

            //Two t for which the component function passes x/y
            } else [[likely]] {

                Vec2<F> t = {
                    (a - b - Math::sqrt(d)) / n,
                    (a - b + Math::sqrt(d)) / n
                };

                bool t0Valid = Math::inRange(t[0], 0, 1);
                bool t1Valid = Math::inRange(t[1], 0, 1);

                u32 solutions = t0Valid + t1Valid;

                switch(solutions) {

                    case 0:
                    default:
                        return {};

                    case 1:

                        if(t0Valid) {
                            return {t[0]};
                        } else {
                            return {t[1]};
                        }

                    case 2:
                        Math::ascOrder(t[0], t[1]);
                        return {t[0], t[1]};

                }

            }

        } else {

            static_assert("Bezier coordinate solutions above degree 2 are unsupported");

        }

    }

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