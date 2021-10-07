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



bool ImageRenderer::init() {

    try {

        imageShader = ShaderLoader::fromFiles(":/shaders/image.avs", ":/shaders/image.afs");

    } catch(std::exception&) {

        Log::error("Image Renderer", "Failed to load image shaders");
        return false;

    }

    std::vector<float> attributeData = VertexHelper::createQuad(1.8, 1.8);
    attributeData.resize(30);

    Timer timer;
    timer.start();

    File fontFile(":/fonts/comic.ttf", File::In | File::Binary);
    fontFile.open();
    FileInputStream fontFileStream(fontFile);
    TrueType::loadFont(fontFileStream);

    Log::info("Timer", "TTF loading time: %fus", timer.getElapsedTime(Time::Unit::Microseconds));

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

    if(isVideo) {

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