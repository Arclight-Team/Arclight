/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 windowconfig.cpp
 */

#include "windowconfig.hpp"

#include "util/assert.hpp"



WindowConfig& WindowConfig::setOpenGLVersion(u32 major, u32 minor) {

	arc_assert(major == 4 && minor <= 6 && minor >= 3, "OpenGL version %d.%d not supported", major, minor);
	this->openglMajor = major;
	this->openglMinor = minor;
	return *this;

}



WindowConfig& WindowConfig::setOpenGLProfile(OpenGLProfile profile) {

	this->profile = profile;
	return *this;

}



WindowConfig& WindowConfig::setOpenGLContextMode(bool forward, bool debug) {

	this->forwardContext = forward;
	this->debugContext = debug;
	return *this;

}



WindowConfig& WindowConfig::setSRGBMode(bool enable) {

	this->srgbRendering = enable;
	return *this;

}



WindowConfig& WindowConfig::setTransparent(bool enable) {

	this->transparent = enable;
	return *this;

}



WindowConfig& WindowConfig::setSamples(u8 samples) {

	this->samples = samples;
	return *this;

}



WindowConfig& WindowConfig::setFramebuffer(u8 redBits, u8 greenBits, u8 blueBits, u8 alphaBits, u8 depthBits, u8 stencilBits) {

	this->redBits = redBits;
	this->greenBits = greenBits;
	this->blueBits = blueBits;
	this->alphaBits = alphaBits;
	this->depthBits = depthBits;
	this->stencilBits = stencilBits;
	return *this;

}



WindowConfig& WindowConfig::setResizable(bool enable) {

	this->resizable = enable;
	return *this;

}



WindowConfig& WindowConfig::setMaximized(bool enable) {

	this->maximized = enable;
	return *this;

}



WindowConfig& WindowConfig::setAlwaysOnTop(bool enable) {

	this->alwaysOnTop = enable;
	return *this;

}