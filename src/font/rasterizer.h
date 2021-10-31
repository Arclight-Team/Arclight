#pragma once

#include "image/image.h"
#include "math/bezier.h"
#include "math/math.h"
#include "debug.h"


namespace Font {

    struct FillBound {
        i32 x;
        bool left;
    };

    void addFillBoundary(std::vector<FillBound>& bounds, i32 x, bool left) {

        auto it = std::find_if(bounds.begin(), bounds.end(), [=](const FillBound& bound) {
            return x < bound.x || (x == bound.x && left > bound.left);
        });

        bounds.insert(it, FillBound(x, left));

    }
   

    template<Pixel P>
    void rasterize(Image<P>& image, const Vec2i& origin, const TrueType::Glyph& glyph, double scale) {

        //Skip if the image size is 0
        if(image.getWidth() == 0 || image.getHeight() == 0) {
            return;
        }

        //The glyph's scaled bounding box size
        Vec2i bounds(Math::floor(glyph.xMax * scale) - Math::floor(glyph.xMin * scale) + 1, Math::floor(glyph.yMax * scale) - Math::floor(glyph.yMin * scale) + 1);

        //Bounding box offset from glyph coordinates to bounding box offset
        Vec2i bbOffset(Math::floor(glyph.xMin * scale), Math::floor(glyph.yMin * scale));

        //Container to store the fills
        std::unordered_map<i32, std::vector<FillBound>> glyphFills;


        //Iterate over all contours
        for(u32 i = 0; i < glyph.contours.size(); i++) {

            //Implicit tangent on-curve point (after bezier spline start)
            Vec2d tangentOnCurve;

            const Vec2ui& contourIndices = glyph.contours[i];

            //Iterate over all points in contour
            for(u32 j = contourIndices.x; j <= contourIndices.y; j++) {

                //Get point indices
                u32 j0 = j;
                u32 j1 = j == contourIndices.y ? contourIndices.x : j + 1;

                bool startOnCurve = glyph.onCurve[j0];
                bool endOnCurve = glyph.onCurve[j1];

                //Straight line
                if(startOnCurve && endOnCurve) {

                    //Scale the points on the line
                    Vec2d start = glyph.points[j0] * scale;
                    Vec2d end = glyph.points[j1] * scale;

                    //True if the contour is rightwound, i.e. pixels on the right need to be colored
                    bool rightwound = end.y >= start.y;

                    //Slope in x and y directions
                    double dx = end.x - start.x;
                    double dy = end.y - start.y;

                    //Swap so that end has the higher y coordinate
                    if(!rightwound) {
                        std::swap(start, end);
                    }

                    //Iterate over each coverage line
                    for(i32 y = static_cast<i32>(Math::floor(start.y + 0.5)); y <= static_cast<i32>(Math::floor(end.y - 0.5)); y++) {

                        //Calculate x intersection
                        i32 x = Math::isZero(dy) ? static_cast<i32>(Math::floor(start.x + rightwound - 0.5)) : static_cast<i32>(Math::floor(dx / dy * (y - start.y) + start.x + rightwound - 0.5));

                        //Add a boundary
                        addFillBoundary(glyphFills[y], x, rightwound);

                    }

                } else if (!endOnCurve) {

                    //Bezier spline
                    Vec2d start = startOnCurve ? glyph.points[j0] * scale : tangentOnCurve;
                    Vec2d control = glyph.points[j1] * scale;

                    //End point
                    u32 j2 = j1 == contourIndices.y ? contourIndices.x : j1 + 1;
                    bool nextOnCurve = glyph.onCurve[j2];
                    Vec2d next = glyph.points[j2] * scale;

                    //Implied tangent on-curve point lies halfway (if next is off-curve)
                    Vec2d end = nextOnCurve ? next : control + (next - control) / 2.0;

                    //Construct the bezier
                    Bezier2d bezier(start, control, end);
                    RectI aabb = bezier.boundingBox().toIntegerRect();

                    bool rightwound = end.y >= start.y;

                    //Solve the y/x problem for each y coordinate inside the BB
                    for(i32 y = aabb.y; y < aabb.getEndY(); y++) {

                        auto xs = bezier.getX(y + 0.5);

                        //Two solutions, two boundaries
                        if(xs[1]) {

                            addFillBoundary(glyphFills[y], static_cast<i32>(Math::floor(*xs[0] + rightwound - 0.5)), rightwound);
                            addFillBoundary(glyphFills[y], static_cast<i32>(Math::floor(*xs[1] + !rightwound - 0.5)), !rightwound);

                        //One solution, single-winding boundary
                        } else if(xs[0]) {

                            addFillBoundary(glyphFills[y], static_cast<i32>(Math::floor(*xs[0] + rightwound - 0.5)), rightwound);

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
        for(auto& [y, fills] : glyphFills) {

            //Skip bad y coords
            u32 py = origin.y + y;

            if(py < 0 || py >= image.getHeight()) {
                continue;
            }

            //Skip all empty ones
            if(fills.size() == 0) {
                continue;
            }

            //Find a valid start left bound
            auto leftIt = std::find_if(fills.begin(), fills.end(), [](const FillBound& fill) {
                return fill.left;
            });

            //Find a valid end right bound
            auto rightIt = std::find_if(fills.rbegin(), fills.rend(), [](const FillBound& fill) {
                return !fill.left;
            });

            //If those iterators reached the end or the first left boundary comes after the first right one, skip
            if(leftIt == fills.end() || rightIt == fills.rend() || leftIt > rightIt.base()) {
                continue;
            }

            //Get the left index and resize accordingly
            //Since resizing doesn't involve a reallocation this is the best approach
            u32 leftIndex = static_cast<u32>(std::distance(fills.begin(), leftIt));
            fills.resize(fills.size() - std::distance(fills.rbegin(), rightIt));

            //Loop over all valid fills
            while(leftIndex < fills.size()) {

                u32 rightIndex = 0;

                //Find the next right border
                for(u32 i = leftIndex + 1; i < fills.size(); i++) {

                    if(!fills[i].left) {
                        rightIndex = i;
                        break;
                    }

                }

                u32 nextLeftIndex = fills.size();

                //Find the next left border
                for(u32 i = rightIndex + 1; i < fills.size(); i++) {

                    if(fills[i].left) {
                        nextLeftIndex = i;
                        break;
                    }

                }
                
                //The correct right border is one before the next left border or end - 1
                rightIndex = nextLeftIndex - 1;

                //Get the border positions
                i32 startX = fills[leftIndex].x;
                i32 endX = fills[rightIndex].x;

                //Update next left border
                leftIndex = nextLeftIndex;

                //Now render the fill
                for(i32 x = startX; x <= endX; x++) {

                    i32 px = origin.x + x;
                    i32 py = origin.y + y;

                    if(px >= 0 && py >= 0 && px < image.getWidth() && py < image.getHeight() && image.getPixel(px, py) == PixelRGB5(0, 0, 0)) {
                        image.setPixel(px, py, PixelRGB5(20, 20, 20));
                    }

                }

            }

        }

    }

}
