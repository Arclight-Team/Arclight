/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 rasterizer.hpp
 */

#pragma once

#include "image/image.hpp"
#include "math/bezier.hpp"
#include "math/math.hpp"



namespace Font {

    struct FillBound {
        double x;
        bool onTransition;
    };

    constexpr void addFillBoundary(std::vector<FillBound>& bounds, double x, bool on) {

        auto it = std::find_if(bounds.begin(), bounds.end(), [=](const FillBound& bound) {
            return x < bound.x;
        });

        bounds.insert(it, FillBound(x, on));

    }
   

    template<Pixel P>
    void rasterizeSingle(Image<P>& image, const Vec2i& origin, const TrueType::Glyph& glyph, double scale, const Mat2d& transform = Mat2d()) {

        //Skip if the image size is 0
        if(image.getWidth() == 0 || image.getHeight() == 0) {
            return;
        }

        //Skip if the glyph is compound
        if(glyph.compound) {
            return;
        }

        //Container to store the fills
        std::unordered_map<i32, std::vector<FillBound>> glyphFills;

        auto glyphData = glyph.getGlyphData();

        //Iterate over all contours
        for(u32 i = 0; i < glyphData.contours.size(); i++) {

            //Implicit tangent on-curve point (after bezier spline start)
            Vec2d tangentOnCurve;

            const Vec2ui& contourIndices = glyphData.contours[i];

            //Iterate over all points in contour
            for(u32 j = contourIndices.x; j <= contourIndices.y; j++) {

                //Get point indices
                u32 j0 = j;
                u32 j1 = j == contourIndices.y ? contourIndices.x : j + 1;

                bool startOnCurve = glyphData.onCurve[j0];
                bool endOnCurve = glyphData.onCurve[j1];

                //Straight line
                if(startOnCurve && endOnCurve) {

                    //Scale the points on the line
                    Vec2d start = (transform * glyphData.points[j0]) * scale;
                    Vec2d end = (transform * glyphData.points[j1]) * scale;

                    if(Math::equal(start.y, end.y)) {

                        i32 y = static_cast<i32>(Math::floor(start.y));
                        
                        if(Math::equal(start.y - y, 0.5)) {

                            addFillBoundary(glyphFills[y], start.x, false);
                            addFillBoundary(glyphFills[y], end.x, true);

                        }

                        continue;
                        
                    }

                    bool onTransition = end.y >= start.y;

                    //Swap so that end has the higher y coordinate
                    if(!onTransition) {
                        std::swap(start, end);
                    }

                    if (Math::equal(start.x, end.x)) {

                        for(i32 y = static_cast<i32>(Math::floor(start.y + 0.4999)); y <= static_cast<i32>(Math::floor(end.y - 0.5)); y++) {

                            addFillBoundary(glyphFills[y], start.x, onTransition);

                        }

                    } else {

                        LineD line(start, end);

                        //Iterate over each coverage line
                        for(i32 y = static_cast<i32>(Math::floor(start.y + 0.4999)); y <= static_cast<i32>(Math::floor(end.y - 0.5)); y++) {

                            //Calculate x intersection
                            double x = line.evaluateInverse(y + 0.5);

                            //Add a boundary
                            addFillBoundary(glyphFills[y], x, onTransition);

                        }

                    }

                } else if (!endOnCurve) {

                    //Bezier spline
                    Vec2d start = startOnCurve ? (transform * glyphData.points[j0]) * scale : tangentOnCurve;
                    Vec2d control = (transform * glyphData.points[j1]) * scale;

                    //End point
                    u32 j2 = j1 == contourIndices.y ? contourIndices.x : j1 + 1;
                    bool nextOnCurve = glyphData.onCurve[j2];
                    Vec2d next = (transform * glyphData.points[j2]) * scale;

                    //Implied tangent on-curve point lies halfway (if next is off-curve)
                    Vec2d end = nextOnCurve ? next : control + (next - control) / 2.0;

                    //Construct the bezier
                    Bezier2d bezier(start, control, end);
                    Bezier1d derivative = bezier.derivative();
                    RectI aabb = bezier.boundingBox().toIntegerRect();

                    //Solve the y/x problem for each y coordinate inside the BB
                    for(i32 y = aabb.y; y <= aabb.getEndY(); y++) {

                        auto ts = bezier.parameterFromY(y + 0.5);

                        //Two solutions, two boundaries
                        if(ts[1]) {

                            double t0On = derivative.evaluate(*ts[0]).y;
                            double t1On = derivative.evaluate(*ts[1]).y;

                            if(!Math::isZero(t0On)) {
                                addFillBoundary(glyphFills[y], bezier.evaluate(*ts[0]).x, t0On > 0);
                            }

                            if(!Math::isZero(t1On)) {
                                addFillBoundary(glyphFills[y], bezier.evaluate(*ts[1]).x, t1On > 0);
                            }

                        //One solution, single-winding boundary
                        } else if(ts[0]) {

                            double tOn = derivative.evaluate(*ts[0]).y;

                            if(!Math::isZero(tOn)){
                                addFillBoundary(glyphFills[y], bezier.evaluate(*ts[0]).x, tOn > 0);
                            }
                            
                        }

                    }
                    
                    if(!nextOnCurve) {
                        //Tangent should be current end point
                        tangentOnCurve = end;
                    } else {
                        //Adjust counter to skip control point
                        j++;
                    }

                } else {

                    //Impossible case, the end point must be on-curve
                    arc_force_assert("Illegal bezier definition in glyph");

                }

            }

        }

        //Fill the fills
        for(const auto& [y, fills] : glyphFills) {

            //Skip bad y coords
            i32 py = origin.y + y;

            if(py < 0 || py >= image.getHeight()) {
                continue;
            }

            //Skip all empty/invalid ones
            if(fills.size() < 2) {
                continue;
            }

            i32 transitionState = 0;
            double lastX = fills[0].x - 1.0;
            bool lastOn = !fills[0].onTransition;

            //Loop over all valid fills
            for(u32 i = 0; i < fills.size(); i++) {

                const FillBound& startFill = fills[i];

                if(Math::equal(startFill.x, lastX) && startFill.onTransition == lastOn) {
                    continue;
                }

                transitionState += startFill.onTransition ? 1 : -1;

                i32 startX = static_cast<i32>(Math::floor(startFill.x + 0.5));
                i32 endX = 0x7FFFFFFF;
                lastX = startFill.x;
                lastOn = startFill.onTransition;

                for(u32 j = i + 1; j < fills.size(); j++) {

                    const FillBound& endFill = fills[j];

                    if(Math::equal(endFill.x, lastX) && startFill.onTransition == lastOn) {
                        continue;
                    }

                    transitionState += endFill.onTransition ? 1 : -1;

                    if(transitionState == 0) {

                        endX = static_cast<i32>(Math::floor(endFill.x - 0.5));
                        lastX = endFill.x;
                        lastOn = endFill.onTransition;
                        i = j;

                        break;

                    }

                }

                //Illegal glyph transition (last contour does not produce a null transition)
                if(endX == 0x7FFFFFFF) {
                    break;
                }

                //Now render the fill
                for(i32 x = Math::max(origin.x + startX, 0); x <= Math::min(origin.x + endX, static_cast<i32>(image.getWidth() - 1)); x++) {
                    image.setPixel(x, py, PixelRGB5(31, 31, 31));
                }

            }

        }

    }


    template<Pixel P>
    void rasterize(const TrueType::Font& font, Image<P>& image, const Vec2i& origin, const TrueType::Glyph& glyph, double scale) {

        if(glyph.compound) {

            for(const TrueType::Glyph::Component& component : glyph.getGlyphComponents()) {
                rasterizeSingle<P>(image, origin + component.offset * scale, font.glyphs[component.glyphIndex], scale, component.transform);
            }

        } else {

            rasterizeSingle<P>(image, origin, glyph, scale);

        }

    }

}