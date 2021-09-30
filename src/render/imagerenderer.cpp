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



const static std::string vsShader = R"(
#version 330 core

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec2 uv;

out vec2 fragUv;

void main() {
    fragUv = uv;
    gl_Position = vec4(vertex, 1.0);
}
)";

const static std::string fsShader = R"(
#version 330 core

uniform sampler2D image;

in vec2 fragUv;
out vec4 color;

void main() {
    color = vec4(texture(image, fragUv).rgb, 1.0);
}
)";



bool ImageRenderer::init() {

    try {

        imageShader = ShaderLoader::fromString(vsShader, fsShader);

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
    image.resize(ImageScaling::Nearest, 160);

    Log::info("", "%f", timer.getElapsedTime());

    GLE::setRowUnpackAlignment(GLE::Alignment::None);
	GLE::setRowPackAlignment(GLE::Alignment::None);

    float lx = 0.5f / image.getWidth();
    float hx = 1 - lx;
    float ly = 0.5f / image.getHeight();
    float hy = 1 - ly;

    auto uvs = {
        lx, ly, hx, ly, hx, hy, lx, ly, hx, hy, lx, hy
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

    imageTextureUnitUniform = imageShader.getUniform("image");

    GLE::enableDepthTests();
    GLE::enableCulling();
    GLE::setClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    Video video;

    return true;

}



void ImageRenderer::render() {

	GLE::clear(GLE::Color | GLE::Depth);
    imageShader.start();

    imageTexture.activate(0);
    imageTextureUnitUniform.setInt(0);

    imageVAO.bind();
    GLE::render(GLE::PrimType::Triangle, 6);

}



void ImageRenderer::destroy() {

    imageShader.destroy();
    imageVBO.destroy();
    imageVAO.destroy();
    imageTexture.destroy();

}