/*
 *	 Copyright (c) 2024 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 WindowRendererGLConfig.cpp
 */

#include "WindowRendererGLConfig.hpp"
#include "Common/Assert.hpp"



WindowRendererGLConfig& WindowRendererGLConfig::setOpenGLVersion(u32 major, u32 minor) {

    this->openglMajor = major;
    this->openglMinor = minor;
    return *this;

}

WindowRendererGLConfig& WindowRendererGLConfig::setOpenGLProfile(OpenGLProfile profile) {

    this->profile = profile;
    return *this;

}

WindowRendererGLConfig& WindowRendererGLConfig::setOpenGLContextMode(bool forward, bool debug) {

    this->forwardContext = forward;
    this->debugContext = debug;
    return *this;

}

WindowRendererGLConfig& WindowRendererGLConfig::setSRGBMode(bool enable) {

    this->srgbRendering = enable;
    return *this;

}

WindowRendererGLConfig& WindowRendererGLConfig::setTransparent(bool enable) {

    this->transparent = enable;
    return *this;

}

WindowRendererGLConfig& WindowRendererGLConfig::setSamples(u8 samples) {

    this->samples = samples;
    return *this;

}

WindowRendererGLConfig& WindowRendererGLConfig::setFramebuffer(u8 redBits, u8 greenBits, u8 blueBits, u8 alphaBits, u8 depthBits, u8 stencilBits) {

    this->redBits = redBits;
    this->greenBits = greenBits;
    this->blueBits = blueBits;
    this->alphaBits = alphaBits;
    this->depthBits = depthBits;
    this->stencilBits = stencilBits;
    return *this;

}

bool WindowRendererGLConfig::isValid() const {
	return (openglMajor == 4 && openglMinor <= 6) || (openglMajor == 3 && openglMinor <= 3);
}
