#pragma once

#include "util/concepts.h"
#include "arcconfig.h"


template<Float F>
class Rectangle {

public:

    constexpr Rectangle() : Rectangle(0, 0, 0, 0) {}
    constexpr Rectangle(const Vec2<F>& pos, const Vec2<F>& size) : Rectangle(pos.x, pos.y, size.x, size.y) {}

    template<Arithmetic A, Arithmetic B, Arithmetic C, Arithmetic D>
    constexpr Rectangle(A x, B y, C w, D h) : x(F(x)), y(F(y)), w(F(w)), h(F(h)) {}

    template<Float G>
    constexpr Rectangle(const Rectangle<G>& rect) : Rectangle(rect.x, rect.y, rect.w, rect.h) {}

    template<Float G>
    constexpr Rectangle<F>& operator=(const Rectangle<G>& other) {

        x = other.x;
        y = other.y;
        w = other.w;
        h = other.h;
        return *this;

    }

    constexpr F getX() const {
        return x;
    }

    constexpr F getY() const {
        return y;
    }

    constexpr F getWidth() const {
        return w;
    }

    constexpr F getHeight() const {
        return h;
    }

    constexpr F getEndX() const {
        return x + w;
    }

    constexpr F getEndY() const {
        return y + h;
    }

    constexpr void setX(F x) {
        this->x = x;
    }

    constexpr void setY(F y) {
        this->y = y;
    }

    constexpr void setWidth(F w) {
        this->w = w;
    }

    constexpr void setHeight(F h) {
        this->h = h;
    }

    
    constexpr Vec2<F> getPosition() const {
        return Vec2<F>(x, y);
    }

    constexpr Vec2<F> getSize() const {
        return Vec2<F>(w, h);
    }

    constexpr Vec2<F> getEndpoint() const {
        return Vec2<F>(x + w, y + h);
    }

    constexpr void setPosition(const Vec2<F>& pos) {
        setPosition(pos.x, pos.y);
    }

    constexpr void setPosition(F x, F y) {
        this->x = x;
        this->y = y;
    }

    constexpr void setSize(const Vec2<F>& size) {
        setSize(size.x, size.y);
    }

    constexpr void setSize(F x, F y) {
        this->w = w;
        this->h = h;
    }


    template<Float G>
    constexpr Rectangle<F> intersect(const Rectangle<G>& other) const {

        F ax0 = x;
        F ax1 = x + w;
        F bx0 = other.x;
        F bx1 = other.x + other.w;
        F ay0 = y;
        F ay1 = y + h;
        F by0 = other.y;
        F by1 = other.y + other.h;

        if(ax0 > bx0) {
            std::swap(ax0, bx0);
            std::swap(ax1, bx1);
        }

        if(ay0 > by0) {
            std::swap(ay0, by0);
            std::swap(ay1, by1);
        }

        Rectangle<F> r;

        //First x
        if(ax1 <= bx0) {

            //No intersection
            return Rectangle<F>;

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
            return Rectangle<F>;

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

    template<Float G>
    constexpr Rectangle<F> intersects(const Rectangle<G>& other) const {
        return !(getEndX() <= other.x || x >= other.getEndX() || getEndY() <= other.y || y >= other.getEndY());
    }

    constexpr F perimeter() const {
        return (w + h) * 2;
    }

    constexpr F area() const {
        return w * h;
    }

    constexpr bool contains(const Vec2<F>& point) const {
        return point.x >= x && point.x <= getEndX() && point.y >= y && point.y <= getEndY();
    }

    template<Float G>
    constexpr Rectangle<F> unite(const Rectangle<G>& other) const {
        return Rectangle<F>::fromPoints(Vec2<F>(Math::min(x, other.x), Math::min(y, other.y)), Vec2<F>(Math::max(x, other.x), Math::max(y, other.y)));
    }


    template<Float G>
    constexpr bool operator==(const Rectangle<G>& other) const {
        return Math::isEqual(x, other.x) && Math::isEqual(y, other.y) && Math::isEqual(w, other.w) && Math::isEqual(h, other.h);
    }

    template<Float G>
    constexpr Rectangle<F>& operator|=(const Rectangle<G>& other) const {
        
        *this = unite(other);
        return *this;

    }

    template<Float G>
    constexpr Rectangle<F>& operator&=(const Rectangle<G>& other) const {
        
        *this = intersect(other);
        return *this;

    }

    constexpr static Rectangle<F> fromPoints(Vec2<F> start, Vec2<F> end) {

        if(start.x > end.x) {
            std::swap(start.x, end.x);
        }

        if(start.y > end.y) {
            std::swap(start.y, end.y);
        }

        return Rectangle<F>(start, end - start);

    }


    F x, y, w, h;

};


template<Float F>
constexpr Rectangle<F> operator|(const Rectangle<F>& a, const Rectangle<F>& b) {
    return a.unite(b);
}

template<Float F>
constexpr Rectangle<F> operator&(const Rectangle<F>& a, const Rectangle<F>& b) {
    return a.intersect(b);
}


using RectF     = Rectangle<float>;
using RectD     = Rectangle<double>;
using RectLD    = Rectangle<long double>;
using RectX     = Rectangle<ARC_STD_FLOAT_TYPE>;
using Rect      = RectX;