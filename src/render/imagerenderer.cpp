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
#include "image/filter/convolution.h"
#include "image/filter/contrast.h"
#include "image/filter/multiply.h"
#include "font/truetype/loader.h"
#include "font/rasterizer.h"
#include "util/unicode.h"
#include "math/line.h"
#include "debug.h"
#include "core/thread/thread.h"
#include "math/bezier.h"
#include "math/fade.h"

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

    fontDirty = false;

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
            double amount = 1.0 / videoFrameCount * i;
            frameImage.applyFilter<MultiplicationFilter>(MultiplicationFilter::Green, amount);
            frameImage.applyFilter<MultiplicationFilter>(MultiplicationFilter::Blue, Math::mod(amount + 0.5, 1));
            video.addFrame(frameImage, i);

        }

    } else if constexpr (renderImageTest) {


        File frameFile("@/textures/noise.bmp", File::In | File::Binary);

        if(!frameFile.open()){
            Log::error("Image Renderer", "Failed to open frame texture");
            return false;
        }

        FileInputStream frameStream(frameFile);
        Image<PixelFmt> frameImage = BMP::loadBitmap<PixelFmt>(frameStream);

        Mat3d convMatrix = {
                0, 0, 0,
                0, 0, 1,
                0, 0, 0
        };

        Mat3d convMatrix2 = {
                0, 0, 0,
                1, 0, 0,
                0, 0, 0
        };

        Mat3d convMatrix3 = {
                0, 1, 0,
                0, 0, 0,
                0, 0, 0
        };

        u32 iterations = 20;
        video.addFrame(frameImage, 0);
        for (int i = 0; i < iterations; i++) {
            video.addFrame(frameImage, i * 10 + 100);
            frameImage.applyFilter<ConvolutionFilter>(convMatrix, ConvolutionFilter::Red);
            frameImage.applyFilter<ConvolutionFilter>(convMatrix2, ConvolutionFilter::Green);
            frameImage.applyFilter<ConvolutionFilter>(convMatrix3, ConvolutionFilter::Blue);
        }

    } else if constexpr (renderFont) {

        Timer timer;

        File fontFile("@/fonts/comic.ttf", File::In | File::Binary);
        fontFile.open();
        FileInputStream fontFileStream(fontFile);

        timer.start();
        font = TrueType::loadFont(fontFileStream);
        Log::info("Timer", "TTF loading time: %fus", timer.getElapsedTime(Time::Unit::Microseconds));

        image = Image<PixelFmt>(canvasWidth, canvasHeight);
        fontScale = 0.1;
        fontDirty = true;
        
        for(u32 i = 0x21; i < 0x80; i++) {
            if(!(i % 0x10)) fontText += "\n";
            fontText += i;
        }

        video.addFrame(image, 0);

    } else if constexpr (renderCanvas) {

        canvasType = 0;

        image = Image<PixelFmt>(canvasWidth, canvasHeight);

        video.addFrame(image, 0);
        recalculateCanvas();

    } else {

        File textureFile("@/textures/what.bmp", File::In | File::Binary);
        
        if(!textureFile.open()){
            Log::error("Image Renderer", "Failed to open image texture");
            return false;
        }

        FileInputStream stream(textureFile);
        image = BMP::loadBitmap<PixelFmt>(stream);
        image.resize(ImageScaling::Bilinear, 160);
        video.addFrame(image, 0);

    }

    imageTextureUnitUniform = imageShader.getUniform("image");
    currentFrameIDUniform = imageShader.getUniform("currentFrameID");

    updateVideo();

    GLE::enableDepthTests();
    GLE::enableCulling();
    GLE::setClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    lastTime = Time::convert(Time::getTimeSinceEpoch(Time::Unit::Nanoseconds), Time::Unit::Nanoseconds, Time::Unit::Seconds);
    video.setSpeed(60);
    video.setReversed(false);
    video.setLooping(true);
    video.restart();

    return true;

}



void ImageRenderer::render() {

    double currentTime = Time::convert(Time::getTimeSinceEpoch(Time::Unit::Nanoseconds), Time::Unit::Nanoseconds, Time::Unit::Seconds);
    double dt  = currentTime - lastTime;
    lastTime = currentTime;

    if(fontDirty) {

        recalculateFont();
        fontDirty = false;

    }

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



void ImageRenderer::recalculateFont() {

    i32 pointY = Math::round(2000 * fontScale);

    i32 caretX = 0;
    i32 caretY = canvasHeight - 1 - pointY;

    Timer timer;
    timer.start();

    image.clear();

    for(auto it = Unicode::begin<Unicode::Encoding::UTF8>(fontText); it != Unicode::end<Unicode::Encoding::UTF8>(fontText); it++) {

        u32 codepoint = *it;
    
        if(codepoint == 0x0A) {

            caretX = 0;
            caretY -= pointY;
            continue;

        }

        u32 glyphIndex = 0;

        if(font.charMap.contains(codepoint)) {
            glyphIndex = font.charMap[codepoint];
        }

        if(glyphIndex >= font.glyphs.size()) {
            glyphIndex = 0;
        }

        const TrueType::Glyph& glyph = font.glyphs[font.charMap[*it]];
        const auto& points = glyph.points;

        Font::rasterize(image, Vec2i(caretX, caretY), glyph, fontScale);

        caretX += static_cast<i32>(Math::round(glyph.advance * fontScale));

    }

    Log::info("Timer", "TTF rendering time: %fus", timer.getElapsedTime(Time::Unit::Microseconds));
    
    video.setFrameImage(0, image);
    updateVideo();

}



void ImageRenderer::recalculateCanvas() {

    Timer t;
    t.start();

    image.clear();

    Bezier3f b;

    for(u32 i = 0; i < 4; i++) {
        b.setControlPoint(i, Vec2f(Random::getRandom().getUint(0, 1999), Random::getRandom().getUint(0, 1099)));
    }

    RectF aabb = b.boundingBox();

    for(u32 i = 0; i <= 10000; i++) {
        
        Vec2f p = b.evaluate(i / 10000.0);
        image.setPixel(p.x, p.y, PixelRGB5(20, 20, 20));

    }

    for(u32 i = aabb.x; i < aabb.x + aabb.w; i++) {
        image.setPixel(i, aabb.y, PixelRGB5(20, 20, 0));
        image.setPixel(i, aabb.getEndpoint().y, PixelRGB5(20, 20, 0));
    }

    for(u32 i = aabb.y; i < aabb.y + aabb.h; i++) {
        image.setPixel(aabb.x, i, PixelRGB5(20, 20, 0));
        image.setPixel(aabb.getEndpoint().x, i, PixelRGB5(20, 20, 0));
    }

    video.setFrameImage(0, image);
    updateVideo();

}


void ImageRenderer::updateVideo() {

    u32 gleLayers = Math::min(video.getFrameCount(), GLE::Limits::getMaxArrayTextureLayers());
    frameTexture.bind();
    frameTexture.setData(video.getWidth(), video.getHeight(), gleLayers, GLE::ImageFormat::RGB8, GLE::TextureSourceFormat::RGBA, GLE::TextureSourceType::UShort1555, nullptr);

    for(u32 i = 0; i < gleLayers; i++) {

        const Image<PixelFmt>& videoFrameImage = video.getFrame(i).getImage();
        frameTexture.update(0, 0, video.getWidth(), video.getHeight(), i, GLE::TextureSourceFormat::RGBA, GLE::TextureSourceType::UShort1555, videoFrameImage.getImageBuffer().data());

    }

    frameTexture.setMagFilter(GLE::TextureFilter::None);
    frameTexture.setMinFilter(GLE::TextureFilter::None);
    frameTexture.generateMipmaps();

}



void ImageRenderer::onScroll(double delta) {

    double scale = Math::exp(delta * 0.01);
    fontScale = Math::clamp(fontScale * scale, 0.001, 1);

    fontDirty = true;

}



void ImageRenderer::dispatchCodepoint(u32 cp) {

    if(cp == 0x08) {

        if(fontText.empty()) {
            return;
        }

        SizeT pos = (--Unicode::end<Unicode::UTF8>(fontText)).getStringPosition();
        fontText.resize(pos);

    } else {

        fontText += Unicode::toUTF<Unicode::UTF8>(cp);

    }

    fontDirty = true;

}



void ImageRenderer::moveCanvas(KeyAction action) {

    switch(action) {

        case KeyAction::Up:
            canvasType = Math::min(canvasType + 1, 26);
            break;

        case KeyAction::Down:
            canvasType = Math::max<i32>(canvasType - 1, 0);
            break;

        default:
            break;

    }

    if constexpr (renderCanvas) {
        recalculateCanvas();
    }

}
