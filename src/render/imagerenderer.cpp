#include "imagerenderer.h"
#include "utility/vertexhelper.h"
#include "utility/shaderloader.h"
#include "util/log.h"
#include "util/file.h"
#include "util/timer.h"
#include "stream/fileinputstream.h"
#include "image/bmp.h"
#include "image/video.h"
#include "image/filter/sepia.h"
#include "image/filter/invert.h"
#include "image/filter/grayscale.h"
#include "image/filter/exponential.h"
#include "image/filter/contrast.h"
#include "font/truetype/loader.h"
#include "font/rasterizer.h"
#include "util/unicode.h"
#include "math/line.h"
#include "debug.h"
#include "core/thread/thread.h"
#include "math/bezier.h"

#include <complex>


bool ImageRenderer::init() {

    try {

        imageShader = ShaderLoader::fromFiles("@/shaders/image.avs", "@/shaders/image.afs");

    } catch(std::exception&) {

        Log::error("Image Renderer", "Failed to load image shaders");
        return false;

    }

    std::vector<float> attributeData = VertexHelper::createQuad(1.8, 1.8);
    attributeData.resize(30);

    GLE::setRowUnpackAlignment(GLE::Alignment::None);
	GLE::setRowPackAlignment(GLE::Alignment::None);

    auto uvs = {
        0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1
    };

    std::copy(uvs.begin(), uvs.end(), &attributeData[18]);

    imageVAO.create();
    imageVAO.bind();

    imageVBO.create();
    imageVBO.bind();
    imageVBO.allocate(attributeData.size() * sizeof(float), attributeData.data());

    imageVAO.enableAttribute(0);
    imageVAO.enableAttribute(1);
    imageVAO.setAttribute(0, 3, GLE::AttributeType::Float, 0, 0);
    imageVAO.setAttribute(1, 2, GLE::AttributeType::Float, 0, 18 * sizeof(float));

    frameTexture.create();
    frameTexture.bind();

    if constexpr (isVideo) {

        for(u32 i = 0; i < videoFrameCount; i++) {

            File frameFile("@/textures/test/frame" + std::to_string(i + 1) + ".bmp", File::In | File::Binary);
        
            if(!frameFile.open()){
                Log::error("Image Renderer", "Failed to open frame texture");
                return false;
            }

            FileInputStream frameStream(frameFile);
            Image<PixelFmt> frameImage = BMP::loadBitmap<PixelFmt>(frameStream);
            frameImage.resize(ImageScaling::Nearest, 256, 192);
            video.addFrame(frameImage, i);

        }

    } else if constexpr (renderFont) {

        Timer timer;

        File fontFile("@/fonts/comic.ttf", File::In | File::Binary);
        fontFile.open();
        FileInputStream fontFileStream(fontFile);
        TrueType::Font font = TrueType::loadFont(fontFileStream);

        Image<PixelFmt> image(2000, 1000);
        std::string text =
R"(Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet.   
Duis autem vel eum iriure dolor in hendrerit in vulputate velit esse molestie consequat, vel illum dolore eu feugiat nulla facilisis at vero eros et accumsan et iusto odio dignissim qui blandit praesent luptatum zzril delenit augue duis dolore te feugait nulla facilisi. Lorem ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy nibh euismod tincidunt ut laoreet dolore magna aliquam erat volutpat.   
Ut wisi enim ad minim veniam, quis nostrud exerci tation ullamcorper suscipit lobortis nisl ut aliquip ex ea commodo consequat. Duis autem vel eum iriure dolor in hendrerit in vulputate velit esse molestie consequat, vel illum dolore eu feugiat nulla facilisis at vero eros et accumsan et iusto odio dignissim qui blandit praesent luptatum zzril delenit augue duis dolore te feugait nulla facilisi.   
Nam liber tempor cum soluta nobis eleifend option congue nihil imperdiet doming id quod mazim placerat facer possim assum. Lorem ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy nibh euismod tincidunt ut laoreet dolore magna aliquam erat volutpat. Ut wisi enim ad minim veniam, quis nostrud exerci tation ullamcorper suscipit lobortis nisl ut aliquip ex ea commodo consequat.   
Duis autem vel eum iriure dolor in hendrerit in vulputate velit esse molestie consequat, vel illum dolore eu feugiat nulla facilisis.   
At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, At accusam aliquyam diam diam dolore dolores duo eirmod eos erat, et nonumy sed tempor et et invidunt justo labore Stet clita ea et gubergren, kasd magna no rebum. sanctus sea sed takimata ut vero voluptua. est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat.   
Consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus.   
Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet.   
Duis autem vel eum iriure dolor in hendrerit in vulputate velit esse molestie consequat, vel illum dolore eu feugiat nulla facilisis at vero eros et accumsan et iusto odio dignissim qui blandit praesent luptatum zzril delenit augue duis dolore te feugait nulla facilisi. Lorem ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy nibh euismod tincidunt ut laoreet dolore magna aliquam erat volutpat.   
Ut wisi enim ad minim veniam, quis nostrud exerci tation ullamcorper suscipit lobortis nisl ut aliquip ex ea commodo consequat. Duis autem vel eum iriure dolor in hendrerit in vulputate velit esse molestie consequat, vel illum dolore eu feugiat nulla facilisis at vero eros et accumsan et iusto odio dignissim qui blandit praesent luptatum zzril delenit augue duis dolore te feugait nulla facilisi.   
Nam liber tempor cum soluta nobis eleifend option congue nihil imperdiet doming id quod mazim placerat facer possim assum. Lorem ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy nibh euismod tincidunt ut laoreet dolore magna aliquam erat volutpat. Ut wisi enim ad minim veniam, quis nostrud exerci tation ullamcorper suscipit lobortis nisl ut aliquip ex ea commodo)";

        u32 caretX = 0;
        u32 caretY = 800;

        timer.start();

        for(auto it = Unicode::begin<Unicode::UTF8>(text); it != Unicode::end<Unicode::UTF8>(text); it++) {

            u32 codepoint = *it;
        
            if(codepoint == 0x0A) {

                caretX = 0;
                caretY -= 40;
                continue;

            }

            u32 glyphIndex = font.charMap[codepoint];

            if(glyphIndex >= font.glyphs.size()) {
                glyphIndex = 0;
            }

            const TrueType::Glyph& glyph = font.glyphs[font.charMap[*it]];
            const auto& points = glyph.points;

            constexpr static double scale = 0.02;
            u32 width = (glyph.xMax - glyph.xMin) * scale;
            u32 height = (glyph.yMax - glyph.yMin) * scale;
            i32 bearing = glyph.bearing * scale;
/*
            Font::rasterize(image, Vec2i(caretX, caretY), glyph, scale);

            i32 bx0 = caretX + Math::floor(glyph.xMin * scale);
            i32 bx1 = caretX + Math::ceil(glyph.xMax * scale);
            i32 by0 = caretY + Math::floor(glyph.yMin * scale);
            i32 by1 = caretY + Math::ceil(glyph.yMax * scale);

            if(bx0 >= 0 && by0 >= 0 && bx1 < image.getWidth() && by1 < image.getHeight()) {

                for(i32 x = bx0; x <= bx1; x++) {

                    image.setPixel(x, by0, PixelRGB5(20, 0, 0));
                    image.setPixel(x, by1, PixelRGB5(20, 0, 0));

                }

                for(i32 y = by0; y <= by1; y++) {

                    image.setPixel(bx0, y, PixelRGB5(20, 0, 0));
                    image.setPixel(bx1, y, PixelRGB5(20, 0, 0));

                }

            }
*/
            caretX += glyph.advance * scale;

        }

        //Bezier2f x(Vec2f(1, 1), Vec2f(200, 800), Vec2f(1400, 467));
        //Bezier3f x(Vec2f(1, 1), Vec2f(200, 800), Vec2f(1400, 467), Vec2f(1800, 98));
        //Bezier4f x(Vec2f(1, 1), Vec2f(200, 800), Vec2f(400, 18), Vec2f(1400, 467), Vec2f(1800, 98));
        //Bezier5f x(Vec2f(1, 1), Vec2f(200, 800), Vec2f(40, 798), Vec2f(1400, 467), Vec2f(400, 18), Vec2f(1800, 98));
        Bezier<11, float> x(Vec2f(1, 1), Vec2f(200, 800), Vec2f(40, 798), Vec2f(1400, 467), Vec2f(400, 18), Vec2f(1800, 98),
                            Vec2f(389, 26), Vec2f(230, 10), Vec2f(1987, 875), Vec2f(1189, 239), Vec2f(1089, 378), Vec2f(771, 590));

        for(u32 i = 0; i < 1000; i++) {
            Vec2f p = x.evaluate(i / 1000.0);
            image.setPixel(p.x, p.y, PixelRGB5(0, 20, 20));
        }

        for(u32 i = 0; i < x.Order + 1; i++) {
            image.setPixel(x.getControlPoint(i).x, x.getControlPoint(i).y, PixelRGB5(20, 20, 20));
        }

        Log::info("Timer", "TTF rendering time: %fus", timer.getElapsedTime(Time::Unit::Microseconds));
        
        video.addFrame(image, 0);

    } else if constexpr (renderCanvas) {

        constexpr static u32 palette[] = {
            0xC11602, 0xCC3709, 0xDC6115, 0xE5851C, 0xE7A51E, 0xD9C620, 0xC0C11C, 0x9FB916, 0x7AAE0E, 0x58A507, 0x00AE01, 0x01BA10, 0x0FCB30, 0x26DE59, 0x3FF286, 0x53FFAE, 
            0x4CFEFC, 0x3ACDED, 0x2B9EE0, 0x1B71D3, 0x174FD0, 0x2230D9, 0x3634DF, 0x4B3AE6, 0x8547F3, 0xB852FF, 0xE358FF, 0xFB4FFB, 0xFF3BE7, 0xF228D0, 0xD9129A, 0xC40069
        };

        for(u32 i = 0; i < 32; i++) {
            colorPalette[i] = PixelConverter::convert<PixelFmt>(PixelRGB8(palette[i] >> 16, (palette[i] & 0xFF00) >> 8, palette[i] & 0xFF));
        }

        canvas = Image<PixelFmt>(canvasWidth, canvasHeight);
        canvasScale = 0.002;
        newCanvasScale = canvasScale;
        canvasPos = Vec2d(0, 0);
        newCanvasPos = canvasPos;

        video.addFrame(canvas, 0);
        recalculateCanvas();

    } else {

        File textureFile("@/textures/what.bmp", File::In | File::Binary);
        
        if(!textureFile.open()){
            Log::error("Image Renderer", "Failed to open image texture");
            return false;
        }

        FileInputStream stream(textureFile);
        canvas = BMP::loadBitmap<PixelFmt>(stream);
        canvas.resize(ImageScaling::Bilinear, 160);
        video.addFrame(canvas, 0);

    }

    imageTextureUnitUniform = imageShader.getUniform("image");
    currentFrameIDUniform = imageShader.getUniform("currentFrameID");

    GLE::enableDepthTests();
    GLE::enableCulling();
    GLE::setClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    updateVideo();

    lastTime = Time::convert(Time::getTimeSinceEpoch(Time::Unit::Nanoseconds), Time::Unit::Nanoseconds, Time::Unit::Seconds);
    video.setSpeed(60);
    video.setReversed(false);
    video.setLooping(false);
    video.restart();

    return true;

}



void ImageRenderer::render() {

    double currentTime = Time::convert(Time::getTimeSinceEpoch(Time::Unit::Nanoseconds), Time::Unit::Nanoseconds, Time::Unit::Seconds);
    double dt  = currentTime - lastTime;
    lastTime = currentTime;

    video.step(dt);

    if(renderCanvas && (newCanvasPos != canvasPos || !Math::isEqual(newCanvasScale, canvasScale))) {

        canvasPos = newCanvasPos;
        canvasScale = newCanvasScale;
        recalculateCanvas();
        updateVideo();

    }

	GLE::clear(GLE::Color | GLE::Depth);
    imageShader.start();

    frameTexture.activate(0);    
    currentFrameIDUniform.setInt(video.getCurrentFrameID());

    imageVAO.bind();
    GLE::render(GLE::PrimType::Triangle, 6);

}



void ImageRenderer::destroy() {

    imageShader.destroy();
    imageVBO.destroy();
    imageVAO.destroy();
    frameTexture.destroy();

}


void ImageRenderer::recalculateCanvas() {

    constexpr static double divergence = 1000;
    constexpr static u32 maxIterations = 100;
    constexpr static u32 type = 0;

    Timer t;
    t.start();

    constexpr static u32 threadCount = 12;
    Thread threads[threadCount];

    auto function = [&](u32 i) {

        u32 startHeight = canvasHeight / threadCount * i;
        u32 endHeight = i == threadCount - 1 ? canvasHeight : startHeight + canvasHeight / threadCount;

        for(u32 y = startHeight; y < endHeight; y++) {

            for(u32 x = 0; x < canvasWidth; x++) {

                const std::complex<double> c((static_cast<double>(x) + canvasPos.x - canvasWidth / 2.0) * canvasScale, (static_cast<double>(y) + canvasPos.y - canvasHeight / 2.0) * canvasScale);
                std::complex<double> z;
                u32 iterations = maxIterations;

                if constexpr (type == 0) {

                    for(u32 i = 0; i < maxIterations; i++) {

                        z = z * z + c;

                        if(std::abs(z) > divergence) {
                            iterations = i;
                            break;
                        }

                    }

                } else if constexpr (type == 1) {

                    i32 lambda = 1;

                    for(u32 i = 0; i < maxIterations; i++) {

                        z = std::pow(z, static_cast<i32>(i) * lambda) + c;

                        if(std::abs(z) > divergence) {
                            iterations = i;
                            break;
                        }

                        lambda *= -1;

                    }

                } else if constexpr (type == 2) {

                    for(u32 i = 0; i < maxIterations; i++) {

                        z = std::sqrt(std::sin(z) + c);

                        if(std::abs(z) > divergence) {
                            iterations = i;
                            break;
                        }

                    }

                } else if constexpr (type == 3) {

                    z = c;

                    for(u32 i = 0; i < maxIterations; i++) {

                        z = std::cos(std::tan(std::sqrt(z)));

                        if(std::abs(z) > divergence) {
                            iterations = i;
                            break;
                        }

                    }

                } else if constexpr (type == 4) {

                    z = c;

                    for(u32 i = 0; i < maxIterations; i++) {

                        z = std::cos(z) / std::asin(z);

                        if(std::abs(z) > divergence) {
                            iterations = i;
                            break;
                        }

                    }

                } else if constexpr (type == 5) {

                    z = c;

                    for(u32 i = 0; i < maxIterations; i++) {

                        z = std::log(z) / std::tan(z);

                        if(std::abs(z) > divergence) {
                            iterations = i;
                            break;
                        }

                    }

                } else if constexpr (type == 6) {

                    z = c;

                    for(u32 i = 0; i < maxIterations; i++) {

                        z = std::cos(z) * std::cos(z) / std::sin(z);

                        if(std::abs(z) > divergence) {
                            iterations = i;
                            break;
                        }

                    }

                }

                u32 color = iterations * 31 / maxIterations;
                canvas.setPixel(x, y, colorPalette[color]);

            }

        }

    };

    for(u32 i = 0; i < threadCount; i++) {
        threads[i].start(function, i);
    }

    for(u32 i = 0; i < threadCount; i++) {
        threads[i].finish();
    }

    Log::info("Fractal Renderer", "Rendering time: %fus", t.getElapsedTime());

    video.setFrameImage(0, canvas);

}


void ImageRenderer::updateVideo() {

    u32 gleLayers = Math::min(video.getFrameCount(), GLE::Limits::getMaxArrayTextureLayers());
    frameTexture.setData(video.getWidth(), video.getHeight(), gleLayers, GLE::ImageFormat::RGB8, GLE::TextureSourceFormat::RGBA, GLE::TextureSourceType::UShort1555, nullptr);

    for(u32 i = 0; i < gleLayers; i++) {

        const Image<PixelFmt>& videoFrameImage = video.getFrame(i).getImage();
        frameTexture.update(0, 0, video.getWidth(), video.getHeight(), i, GLE::TextureSourceFormat::RGBA, GLE::TextureSourceType::UShort1555, videoFrameImage.getImageBuffer().data());

    }

    frameTexture.setMagFilter(GLE::TextureFilter::None);
    frameTexture.setMinFilter(GLE::TextureFilter::None);
    frameTexture.generateMipmaps();

}


void ImageRenderer::moveCanvas(KeyAction action) {

    constexpr static Vec2d viewMotion(1, 1);
    constexpr static double viewScale = 0.8;

    switch(action) {

        case KeyAction::Up:
            newCanvasPos.y += viewMotion.y;
            break;

        case KeyAction::Down:
            newCanvasPos.y -= viewMotion.y;
            break;

        case KeyAction::Left:
            newCanvasPos.x -= viewMotion.x;
            break;

        case KeyAction::Right:
            newCanvasPos.x += viewMotion.x;
            break;

        case KeyAction::ZoomIn:
            newCanvasScale *= viewScale;
            break;

        case KeyAction::ZoomOut:
            newCanvasScale /= viewScale;
            break;

    }

}
