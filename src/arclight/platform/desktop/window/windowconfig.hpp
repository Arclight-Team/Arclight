/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 windowconfig.hpp
 */

#pragma once

#include "types.hpp"



class WindowConfig {

public:

	enum class OpenGLProfile {
		Core,
		Compatibility,
		Any
	};

	constexpr WindowConfig() :
		openglMajor(3), openglMinor(3), profile(OpenGLProfile::Core), forwardContext(true), debugContext(false), srgbRendering(true),
		samples(1), redBits(8), greenBits(8), blueBits(8), alphaBits(8), depthBits(24), stencilBits(8),
		resizable(false), maximized(false), alwaysOnTop(false), transparent(false) {}

	WindowConfig& setOpenGLVersion(u32 major, u32 minor);
	WindowConfig& setOpenGLProfile(OpenGLProfile profile);
	WindowConfig& setOpenGLContextMode(bool forward, bool debug);
	WindowConfig& setSRGBMode(bool enable);
	WindowConfig& setTransparent(bool enable);
	WindowConfig& setSamples(u8 samples);
	WindowConfig& setFramebuffer(u8 redBits, u8 greenBits, u8 blueBits, u8 alphaBits, u8 depthBits, u8 stencilBits);
	WindowConfig& setResizable(bool enable);
	WindowConfig& setMaximized(bool enable);
	WindowConfig& setAlwaysOnTop(bool enable);

private:

	friend class Window;

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