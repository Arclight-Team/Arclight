#pragma once

#include "renderer.h"
#include "gle/gle.h"
#include "math/math.h"
#include "image/video.h"


class ImageRenderer : public Renderer {

public:

	virtual bool init() override;
	virtual void render() override;
	virtual void destroy() override;

private:

    GLE::ShaderProgram imageShader;
    GLE::VertexBuffer imageVBO;
    GLE::VertexArray imageVAO;

    GLE::Texture2D imageTexture;
    GLE::ArrayTexture2D frameTexture;
    GLE::Uniform imageTextureUnitUniform;
    GLE::Uniform currentFrameIDUniform;

    constexpr static Pixel PixelFormat = Pixel::RGB5;

    Video<PixelFormat> video;
    double lastTime;

    constexpr static u32 videoFrameCount = 512;

};