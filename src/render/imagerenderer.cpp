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


bool ImageRenderer::init() {


    LineF l(Vec2d(2, 3), Vec2f(4.3, 4));
    LineF ll(Vec2f(0, 0), Vec2f(1, 2));
    auto x = l.intersection(ll);

    if(x) {
        ArcDebug() << *x;
    }

    try {

        imageShader = ShaderLoader::fromFiles(":/shaders/image.avs", ":/shaders/image.afs");

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

            File frameFile(":/textures/test/frame" + std::to_string(i + 1) + ".bmp", File::In | File::Binary);
        
            if(!frameFile.open()){
                Log::error("Image Renderer", "Failed to open frame texture");
                return false;
            }

            FileInputStream frameStream(frameFile);
            Image<PixelFormat> frameImage = BMP::loadBitmap<PixelFormat>(frameStream);
            frameImage.resize(ImageScaling::Nearest, 256, 192);
            video.addFrame(frameImage, i);

        }

    } else if constexpr (renderFont) {

        Timer timer;

        File fontFile(":/fonts/comic.ttf", File::In | File::Binary);
        fontFile.open();
        FileInputStream fontFileStream(fontFile);
        TrueType::Font font = TrueType::loadFont(fontFileStream);

        Image<PixelFormat> image(2000, 1000);
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

            caretX += glyph.advance * scale;

        }

        Log::info("Timer", "TTF rendering time: %fus", timer.getElapsedTime(Time::Unit::Microseconds));
        
        video.addFrame(image, 0);

    } else {

        File textureFile(":/textures/what.bmp", File::In | File::Binary);
        
        if(!textureFile.open()){
            Log::error("Image Renderer", "Failed to open image texture");
            return false;
        }

        FileInputStream stream(textureFile);
        Image<PixelFormat> image = BMP::loadBitmap<PixelFormat>(stream);
        image.resize(ImageScaling::Bilinear, 160);
        video.addFrame(image, 0);

    }

    u32 gleLayers = Math::min(video.getFrameCount(), GLE::Limits::getMaxArrayTextureLayers());
    frameTexture.setData(video.getWidth(), video.getHeight(), gleLayers, GLE::ImageFormat::RGB8, GLE::TextureSourceFormat::RGBA, GLE::TextureSourceType::UShort1555, nullptr);

    for(u32 i = 0; i < gleLayers; i++) {

        const Image<PixelFormat>& videoFrameImage = video.getFrame(i).getImage();
        frameTexture.update(0, 0, video.getWidth(), video.getHeight(), i, GLE::TextureSourceFormat::RGBA, GLE::TextureSourceType::UShort1555, videoFrameImage.getImageBuffer().data());

    }

    frameTexture.setMagFilter(GLE::TextureFilter::None);
    frameTexture.setMinFilter(GLE::TextureFilter::None);
    frameTexture.generateMipmaps();

    imageTextureUnitUniform = imageShader.getUniform("image");
    currentFrameIDUniform = imageShader.getUniform("currentFrameID");

    GLE::enableDepthTests();
    GLE::enableCulling();
    GLE::setClearColor(0.1f, 0.1f, 0.1f, 1.0f);

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