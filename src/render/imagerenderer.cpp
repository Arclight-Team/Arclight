#include "imagerenderer.h"
#include "utility/vertexhelper.h"
#include "utility/shaderloader.h"
#include "util/log.h"

#include "util/file.h"
#include "stream/fileinputstream.h"
#include "image/bmp.h"



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

    } catch(std::exception& e) {

        Log::error("Image Renderer", "Failed to load image shaders");
        return false;

    }

    std::vector<float> attributeData = VertexHelper::createQuad(1.8, 1.8);
    attributeData.resize(30);
    
    auto uvs = {
        0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1
    };

    std::copy(uvs.begin(), uvs.end(), &attributeData[18]);

    File textureFile(":/textures/test.bmp", File::In | File::Binary);
    
    if(!textureFile.open()){
        Log::error("Image Renderer", "Failed to open image texture");
        return false;
    }

    FileInputStream stream(textureFile);
    Image image = BMP::loadBitmap<Pixel::RGB8>(stream);

    GLE::setRowUnpackAlignment(GLE::Alignment::None);
	GLE::setRowPackAlignment(GLE::Alignment::None);

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
    imageTexture.setData(image.getWidth(), image.getHeight(), GLE::ImageFormat::RGB8, GLE::TextureSourceFormat::RGB, GLE::TextureSourceType::UByte, image.getImageBuffer().data());
    imageTexture.generateMipmaps();

    imageTextureUnitUniform = imageShader.getUniform("image");

    GLE::enableDepthTests();
    GLE::enableCulling();

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