#pragma once

#include "renderer.h"
#include "gle/gle.h"
#include "math/math.h"
#include "image/video.h"
#include "font/truetype/truetype.h"


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

    void onScroll(double delta);
    void dispatchCodepoint(u32 cp);
    void moveCanvas(KeyAction action);

private:

    void recalculateFont();
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
    Image<PixelFmt> image;
    double lastTime;

    TrueType::Font font;
    std::string fontText;
    double fontScale;

    constexpr static u32 videoFrameCount = 12;
    constexpr static bool isVideo = false;
    constexpr static bool renderFont = true;
    constexpr static bool renderCanvas = false;
    constexpr static bool renderImageTest = false;

    constexpr static u32 canvasWidth = 2000;
    constexpr static u32 canvasHeight = 1100;

    u32 canvasType;

};