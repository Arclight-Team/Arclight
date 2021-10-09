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
#include "debug.h"
#include "util/unicode.h"


bool ImageRenderer::init() {

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
        timer.start();

        File fontFile(":/fonts/comic.ttf", File::In | File::Binary);
        fontFile.open();
        FileInputStream fontFileStream(fontFile);
        TrueType::Font font = TrueType::loadFont(fontFileStream);

        Image<PixelFormat> image(2000, 1000);
        std::string text = "This sample text demonstrates bearings";
        u32 caretX = 200;
        u32 caretY = 100;
        UnicodeIterator unicodeIt = Unicode::begin<Unicode::UTF8>(text);

        for(auto it = Unicode::begin<Unicode::UTF8>(text); it != Unicode::end<Unicode::UTF8>(text); it++) {

            Log::info("", "%d", *it);

            const TrueType::Glyph& glyph = font.glyphs[font.charMap[*it]];
            const auto& points = glyph.points;

            constexpr static u32 divisor = 25;
            u32 width = (glyph.xMax - glyph.xMin) / divisor;
            u32 height = (glyph.yMax - glyph.yMin) / divisor;

            for(u32 j = 0; j < points.size(); j++) {

                const Vec2i& point = points[j];
                i32 x = caretX + (glyph.bearing + point.x) / divisor;
                i32 y = caretY + point.y / divisor;
                
                if(x >= 0 && y >= 0 && x < image.getWidth() && y < image.getHeight()) {
                    image.setPixel(x, y, PixelRGB5(20, 20, 20));
                }

            }
                    
            image.setPixel(caretX, caretY, PixelRGB5(20, 0, 0));
            image.setPixel(caretX + width, caretY, PixelRGB5(20, 0, 0));
            image.setPixel(caretX, caretY + height, PixelRGB5(20, 0, 0));
            image.setPixel(caretX + width, caretY + height, PixelRGB5(20, 0, 0));

            caretX += glyph.advance / divisor;

        }

        video.addFrame(image, 0);

        Log::info("Timer", "TTF loading time: %fus", timer.getElapsedTime(Time::Unit::Microseconds));

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