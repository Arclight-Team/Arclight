#pragma once

#include "image/image.hpp"
#include "math/matrix.hpp"
#include "types.hpp"


// Convolution filter with a 3x3 matrix
class ConvolutionFilter {

public:

    enum EdgeHandling {
        Repeat,
        Clamp,
        Ignore
    };

    enum Channels {
        Red   = 1,
        Green = 2,
        Blue  = 4,
        Alpha = 8
    };

    template<Pixel P>
    constexpr static void run(Image<P>& image, Mat3<double> convMat, u32 channels = Red | Green | Blue, EdgeHandling edgeType = Ignore) {

        if (!channels) {
            return;
        }

        using Format = typename Image<P>::Format;

        Image<P> buffer(image.getWidth() + 2, image.getHeight() + 2);
        image.copy(buffer, { 0, 0, image.getWidth(), image.getHeight() }, { 1, 1 });

        auto function = [&]<EdgeHandling E>() {

            for(u32 y = 0; y < image.getHeight(); y++) {

                for(u32 x = 0; x < image.getWidth(); x++) {

                    double r = 0;
                    double g = 0;
                    double b = 0;
                    double a = 0;

                    // Used in the end to normalize the output
                    double sumChecked = 0;

                    auto& p = image.getPixel(x, y);

                    if (!(channels & Red)) {
                        r = p.getRed();
                    }
                    if (!(channels & Green)) {
                        g = p.getGreen();
                    }
                    if (!(channels & Blue)) {
                        b = p.getBlue();
                    }
                    if (!(channels & Alpha)) {
                        a = p.getAlpha();
                    }

//                    Log::info("ConvFilter", "(%i, %i)", x, y);
                    for (u32 offY = 0; offY < 3; offY++) {
                        for (u32 offX = 0; offX < 3; offX++) {
                            calcPX<P, E>(buffer, convMat, channels, x + offX, y + offY, offX, offY, r, g, b, a, sumChecked);
//                            Log::info("ConvFilter", "(%i, %i, %i), (%i, %i), %i", r, g, b, offX, offY, sumChecked);
                        }
                    }

                    // Calculate the average
                    if (channels & Red) {
                        r = Math::abs(r / sumChecked);
                    }
                    if (channels & Green) {
                        g = Math::abs(g / sumChecked);
                    }
                    if (channels & Blue) {
                        b = Math::abs(b / sumChecked);
                    }
                    if (channels & Alpha) {
                        a = Math::abs(a / sumChecked);
                    }

                    image.getPixel(x, y).setRGBA(r + 0.5, g + 0.5, b + 0.5, a);

                }

            }

        };

        switch(edgeType) {

            case EdgeHandling::Ignore:
                function.template operator()<EdgeHandling::Ignore>();
                break;

            case EdgeHandling::Clamp:
                // Set up the borders of the buffer image so we don't have to worry about them anymore
                buffer.copy( { 1, 1, buffer.getWidth() - 2, 1 }, { 1, 0 } );
                buffer.copy( { 1, buffer.getHeight() - 2, buffer.getWidth() - 2, 1 }, { 1, buffer.getHeight() - 1 } );
                buffer.copy( { 1, 0, 1, buffer.getHeight() }, { 0, 0 } );
                buffer.copy( { buffer.getWidth() - 2, 0, 1, buffer.getHeight() }, { buffer.getWidth() - 1, 0 } );

                function.template operator()<EdgeHandling::Clamp>();
                break;

            default:
            case EdgeHandling::Repeat:
                // Set up the borders of the buffer image so we don't have to worry about them anymore
                buffer.copy( { 1, 1, buffer.getWidth() - 2, 1 }, { 1, buffer.getHeight() - 1 } );
                buffer.copy( { 1, buffer.getHeight() - 2, buffer.getWidth() - 2, 1 }, { 1, 0 } );
                buffer.copy( { 1, 0, 1, buffer.getHeight() }, { buffer.getWidth() - 1, 0 } );
                buffer.copy( { buffer.getWidth() - 2, 0, 1, buffer.getHeight() }, { 0, 0 } );

                function.template operator()<EdgeHandling::Repeat>();
                break;

        }

    }

private:

    // Multiplies the pixel with the corresponding entry in the matrix. Expects all pixel values to be valid
    template<Pixel P, EdgeHandling E>
    constexpr static void calcPX(Image<P>& buffer, Mat3<double> convMat, u32 channels, u32 imX, u32 imY, u32 matX, u32 matY, double& r, double& g, double& b, double& a, double& checked) {
        if constexpr (E == Ignore) {
            if (imX == 0 || imX == buffer.getWidth() - 1 || imY == 0 || imY == buffer.getHeight() - 1) {
                return;
            }
        }

        auto& p = buffer.getPixel(imX, imY);

        if (channels & Red) {
            r += convMat[matX][matY] * p.getRed();
        }
        if (channels & Green) {
            g += convMat[matX][matY] * p.getGreen();
        }
        if (channels & Blue) {
            b += convMat[matX][matY] * p.getBlue();
        }
        if (channels & Alpha) {
            a += convMat[matX][matY] * p.getAlpha();
        }

        checked += Math::abs(convMat[matX][matY]);
    }
};