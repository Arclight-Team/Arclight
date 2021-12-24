#pragma once

#include "math.hpp"



namespace Fade {


    //Linear
    template<Arithmetic T>
    constexpr auto linearIn(T t) {
        return t;
    }

    template<Arithmetic T>
    constexpr auto linearOut(T t) {
        return t;
    }

    template<Arithmetic T>
    constexpr auto linearInOut(T t) {
        return t;
    }


    //Quadratic
    template<Arithmetic T>
    constexpr auto quadIn(T t) {
        return Math::pow(t, 2);
    }

    template<Arithmetic T>
    constexpr auto quadOut(T t) {
        return -Math::pow(t - 1, 2) + 1;
    }

    template<Arithmetic T>
    constexpr auto quadInOut(T t) {
        return t > 0.5 ? -2 * Math::pow(t - 1, 2) + 1 : 2 * Math::pow(t, 2);
    }


    //Cubic
    template<Arithmetic T>
    constexpr auto cubicIn(T t) {
        return Math::pow(t, 3);
    }

    template<Arithmetic T>
    constexpr auto cubicOut(T t) {
        return 1 + Math::pow(t - 1, 3);
    }

    template<Arithmetic T>
    constexpr auto cubicInOut(T t) {
        return t > 0.5 ? 4 * Math::pow(t - 1, 3) + 1 : 4 * Math::pow(t, 3);
    }

    
    //Quartic
    template<Arithmetic T>
    constexpr auto quarticIn(T t) {
        return Math::pow(t, 4);
    }

    template<Arithmetic T>
    constexpr auto quarticOut(T t) {
        return 1 - Math::pow(t - 1, 4);
    }

    template<Arithmetic T>
    constexpr auto quarticInOut(T t) {
        return t > 0.5 ? -8 * Math::pow(t - 1, 4) + 1 : 8 * Math::pow(t, 4);
    }

        
    //Quintic
    template<Arithmetic T>
    constexpr auto quinticIn(T t) {
        return Math::pow(t, 5);
    }

    template<Arithmetic T>
    constexpr auto quinticOut(T t) {
        return 1 + Math::pow(t - 1, 5);
    }

    template<Arithmetic T>
    constexpr auto quinticInOut(T t) {
        return t > 0.5 ? 16 * Math::pow(t - 1, 5) + 1 : 16 * Math::pow(t, 5);
    }


    //Exponential
    template<Arithmetic T>
    constexpr auto expIn(T t, T b = 5) {
        
        T eb = Math::exp(b);
        T ebx = Math::pow(eb, t);
        return (ebx - 1) / (eb - 1);
        
    }

    template<Arithmetic T>
    constexpr auto expOut(T t, T b = 5) {

        T eb = Math::exp(-b);
        T ebx = Math::pow(eb, t);
        return (ebx - 1) / (eb - 1);

    }

    template<Arithmetic T>
    constexpr auto expInOut(T t, T b = 5) {
        return t > 0.5 ? 0.5 * expOut(2 * t - 1, b) + 0.5 : 0.5 * expIn(2 * t, b);
    }


    //Circular
    template<Arithmetic T>
    constexpr auto circIn(T t) {
        return 1 - Math::sqrt(1 - Math::pow(t, 2));
    }

    template<Arithmetic T>
    constexpr auto circOut(T t) {
        return Math::sqrt(1 - Math::pow(t - 1, 2));
    }

    template<Arithmetic T>
    constexpr auto circInOut(T t) {
        return t > 0.5 ? Math::sqrt(0.25 - Math::pow(t - 1, 2)) + 0.5 : 0.5 - Math::sqrt(0.25 - Math::pow(t, 2));
    }

    
    //Back
    template<Arithmetic T>
    constexpr auto backIn(T t, T b = 1.5) {
        return (b + 1) * Math::pow(t, 3) - b * Math::pow(t, 2);
    }

    template<Arithmetic T>
    constexpr auto backOut(T t, T b = 1.5) {
        return (b + 1) * Math::pow(t - 1, 3) + b * Math::pow(t - 1, 2) + 1;
    }

    template<Arithmetic T>
    constexpr auto backInOut(T t, T b = 1.5) {
        return t > 0.5 ? 4 * (b + 1) * Math::pow(t - 1, 3) + 2 * b * Math::pow(t - 1, 2) + 1 : 4 * (b + 1) * Math::pow(t, 3) - 2 * b * Math::pow(t, 2);
    }


    //Elastic
    template<Arithmetic T>
    constexpr auto elasticIn(T t, T b = 5, T c = 3) {
        return (1 - expOut(1 - t, b)) * Math::cos(c * Math::pi * (t - 1));
    }

    template<Arithmetic T>
    constexpr auto elasticOut(T t, T b = 5, T c = 3) {
        return (expOut(t, b) - 1) * Math::cos(c * Math::pi * t) + 1;
    }

    /*
        Note: This function is sigmoidal on the halfway point. This is to keep the continuity of parameter c.
    */
    template<Arithmetic T>
    constexpr auto elasticInOut(T t, T b = 5, T c = 5) {
        return t > 0.5 ? 0.5 * elasticOut(2 * t - 1, b, c) + 0.5 : 0.5 * elasticIn(2 * t, b, c);
    }


    //Polynomial
    template<Arithmetic T>
    constexpr auto polyIn(T t, T n) {
        return Math::pow(t, n);
    }

    template<Arithmetic T>
    constexpr auto polyOut(T t, T n) {
        return 1 - Math::pow(1 - t, n);
    }

    template<Arithmetic T>
    constexpr auto polyInOut(T t, T n) {
        return t > 0.5 ? 1 - 0.5 * Math::pow(1.5 - t, n) : 0.5 * Math::pow(2 * t, n);
    }


    //Sine
    template<Arithmetic T>
    constexpr auto sineIn(T t) {
        return 1 - Math::cos(t * Math::pi / 2);
    }

    template<Arithmetic T>
    constexpr auto sineOut(T t) {
        return Math::sin(t * Math::pi / 2);
    }

    template<Arithmetic T>
    constexpr auto sineInOut(T t) {
        return -(Math::cos(t * Math::pi) - 1) / 2;
    }


}