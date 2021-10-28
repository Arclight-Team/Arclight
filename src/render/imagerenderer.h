#pragma once

#include "renderer.h"
#include "gle/gle.h"
#include "math/math.h"
#include "image/video.h"


class ImageRenderer : public Renderer {

public:

    enum class KeyAction {
        Up,
        Down,
        Left,
        Right,
        ZoomIn,
        ZoomOut
    };

	virtual bool init() override;
	virtual void render() override;
	virtual void destroy() override;

    void moveCanvas(KeyAction action);

private:

    void recalculateCanvas();
    void updateVideo();

    GLE::ShaderProgram imageShader;
    GLE::VertexBuffer imageVBO;
    GLE::VertexArray imageVAO;

    GLE::ArrayTexture2D frameTexture;
    GLE::Uniform imageTextureUnitUniform;
    GLE::Uniform currentFrameIDUniform;

    constexpr static Pixel PixelFmt = Pixel::RGB5;

    Video<PixelFmt> video;
    double lastTime;

    constexpr static u32 videoFrameCount = 12;
    constexpr static bool isVideo = true;
    constexpr static bool renderFont = false;
    constexpr static bool renderCanvas = false;
    constexpr static bool renderImageTest = true;

    constexpr static u32 canvasWidth = 2000;
    constexpr static u32 canvasHeight = 1100;

    Image<PixelFmt> canvas;
    u32 canvasType;

};