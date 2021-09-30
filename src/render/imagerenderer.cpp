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



bool ImageRenderer::init() {

    try {

        imageShader = ShaderLoader::fromFiles(":/shaders/image.avs", ":/shaders/image.afs");

    } catch(std::exception&) {

        Log::error("Image Renderer", "Failed to load image shaders");
        return false;

    }

    std::vector<float> attributeData = VertexHelper::createQuad(1.8, 1.8);
    attributeData.resize(30);

    File textureFile(":/textures/test.bmp", File::In | File::Binary);
    
    if(!textureFile.open()){
        Log::error("Image Renderer", "Failed to open image texture");
        return false;
    }

    FileInputStream stream(textureFile);
    Image image = BMP::loadBitmap<Pixel::RGB5>(stream);

    Timer timer;
    timer.start();

    //image.applyFilter<GrayscaleFilter>();
    //image.applyFilter<SepiaFilter>();
    //image.applyFilter<ExponentialFilter>(2);
    //image.applyFilter<ContrastFilter>(1);
    //image.applyFilter<InversionFilter>();
    image.resize(ImageScaling::Bilinear, 160);

    Log::info("", "%f", timer.getElapsedTime());

    GLE::setRowUnpackAlignment(GLE::Alignment::None);
	GLE::setRowPackAlignment(GLE::Alignment::None);

    float lx = 0.5f / image.getWidth();
    float hx = 1 - lx;
    float ly = 0.5f / image.getHeight();
    float hy = 1 - ly;

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

    imageTexture.create();
    imageTexture.bind();
    //imageTexture.setData(image.getWidth(), image.getHeight(), GLE::ImageFormat::RGB8, GLE::TextureSourceFormat::RGB, GLE::TextureSourceType::UByte, image.getImageBuffer().data());
    imageTexture.setData(image.getWidth(), image.getHeight(), GLE::ImageFormat::RGB8, GLE::TextureSourceFormat::RGBA, GLE::TextureSourceType::UShort1555, image.getImageBuffer().data());
    imageTexture.setMagFilter(GLE::TextureFilter::None);
    imageTexture.setMinFilter(GLE::TextureFilter::None);
    imageTexture.generateMipmaps();

    frameTexture.create();
    frameTexture.bind();

    for(u32 i = 0; i < 12; i++) {

        File frameFile(":/textures/test" + std::to_string(i + 1) + ".bmp", File::In | File::Binary);
    
        if(!frameFile.open()){
            Log::error("Image Renderer", "Failed to open frame texture");
            return false;
        }

        FileInputStream frameStream(frameFile);
        Image frameImage = BMP::loadBitmap<Pixel::RGB8>(frameStream);
        video.addFrame(frameImage, i);

    }

    frameTexture.setData(video.getWidth(), video.getHeight(), video.getFrameCount(), GLE::ImageFormat::RGB8, GLE::TextureSourceFormat::RGB, GLE::TextureSourceType::UByte, nullptr);

    for(u32 i = 0; i < video.getFrameCount(); i++) {

        const Image<>& videoFrameImage = video.getFrame(i).getImage();
        frameTexture.update(0, 0, video.getWidth(), video.getHeight(), i, GLE::TextureSourceFormat::RGB, GLE::TextureSourceType::UByte, videoFrameImage.getImageBuffer().data());

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
    video.setSpeed(30);
    //video.setReversed(true);
    video.setLooping(true);
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

    imageTexture.activate(0);
    imageTextureUnitUniform.setInt(0);
    currentFrameIDUniform.setInt(video.getCurrentFrameID());

    imageVAO.bind();
    GLE::render(GLE::PrimType::Triangle, 6);

}



void ImageRenderer::destroy() {

    imageShader.destroy();
    imageVBO.destroy();
    imageVAO.destroy();
    imageTexture.destroy();

}