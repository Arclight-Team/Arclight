/*
 *	 Copyright (c) 2024 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 WindowRendererGLConfig.hpp
 */

#pragma once

#include "Common/Types.hpp"



class WindowRendererGLConfig {

public:

    enum class OpenGLProfile {
        Core,
        Compatibility,
        Any
    };

    constexpr WindowRendererGLConfig() : openglMajor(4), openglMinor(3), profile(OpenGLProfile::Core),
										forwardContext(true), debugContext(false), srgbRendering(true),
										transparent(false),
										samples(1), redBits(8), greenBits(8), blueBits(8), alphaBits(8), depthBits(24),
										stencilBits(8), resizable(false), maximized(false), alwaysOnTop(false) {}

    WindowRendererGLConfig& setOpenGLVersion(u32 major, u32 minor);
    WindowRendererGLConfig& setOpenGLProfile(OpenGLProfile profile);
    WindowRendererGLConfig& setOpenGLContextMode(bool forward, bool debug);
    WindowRendererGLConfig& setSRGBMode(bool enable);
    WindowRendererGLConfig& setTransparent(bool enable);
    WindowRendererGLConfig& setSamples(u8 samples);
    WindowRendererGLConfig& setFramebuffer(u8 redBits, u8 greenBits, u8 blueBits, u8 alphaBits, u8 depthBits, u8 stencilBits);

	bool isValid() const;

    u32 openglMajor;
    u32 openglMinor;
    OpenGLProfile profile;
    bool forwardContext;
    bool debugContext;
    bool srgbRendering;
    bool transparent;
    u32 samples;
    u8 redBits;
    u8 greenBits;
    u8 blueBits;
    u8 alphaBits;
    u8 depthBits;
    u8 stencilBits;
    bool resizable;
    bool maximized;
    bool alwaysOnTop;

};
