/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 glecore.cpp
 */

#include "glecore.hpp"

#include GLE_HEADER


GLE_BEGIN

namespace {

	u32 openglContextVersion = 0;
	bool openglDebugContext = false;

	u32 maxTextureSize = 0;
	u32 max3DTextureSize = 0;
	u32 maxArrayTextureLayers = 0;
	u32 maxMipmapLevels = 0;
	float maxAnisotropy = 1.0;
	u32 maxTextureUnits = 0;

	u32 maxDepthSamples = 0;
	u32 maxColorSamples = 0;
	u32 maxIntegerSamples = 0;

	u32 maxColorAttachments = 0;
	u32 maxDrawBuffers = 0;

	u32 maxUniformBlockBindings = 0;

}


namespace Core {

	bool init() {

		glewExperimental = GL_TRUE;
		GLenum result = glewInit();

		if (result != GLEW_OK) {
			error("OpenGL context creation failed: %s", glewGetErrorString(result));
			return false;
		}

		i32 majorVersion = 0;
		i32 minorVersion = 0;
		i32 flags = 0;

		glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
		glGetIntegerv(GL_MINOR_VERSION, &minorVersion);
		glGetIntegerv(GL_CONTEXT_FLAGS, &flags);

		openglContextVersion = majorVersion << 8 | minorVersion;
		openglDebugContext = flags & GL_CONTEXT_FLAG_DEBUG_BIT;

		GLE::info("OpenGL %d.%d context set up", majorVersion, minorVersion);
		GLE::info("Debug context %s", (openglDebugContext ? "enabled" : "disabled"));

		i32 tmp = 0;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &tmp);
		maxTextureSize = tmp;
		glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &tmp);
		max3DTextureSize = tmp;
		glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &tmp);
		maxArrayTextureLayers = tmp;

		u32 texSize = maxTextureSize;
		maxMipmapLevels = 0;

		while (texSize != 1) {
			texSize /= 2;
			maxMipmapLevels++;
		}

		if (GLE_EXT_SUPPORTED(ARB_texture_filter_anisotropic)) {
			glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAnisotropy);
		}

		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &tmp);
		maxTextureUnits = tmp;

		glGetIntegerv(GL_MAX_DEPTH_TEXTURE_SAMPLES, &tmp);
		maxDepthSamples = tmp;
		glGetIntegerv(GL_MAX_COLOR_TEXTURE_SAMPLES, &tmp);
		maxColorSamples = tmp;
		glGetIntegerv(GL_MAX_INTEGER_SAMPLES, &tmp);
		maxIntegerSamples = tmp;

		glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &tmp);
		maxColorAttachments = tmp;
		glGetIntegerv(GL_MAX_DRAW_BUFFERS, &tmp);
		maxDrawBuffers = tmp;

		glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &tmp);
		maxUniformBlockBindings = tmp;

		return true;

	}


	void printErrors() {

	#ifndef GLE_DISABLE_DEBUG

		GLenum errorNumber = 0;

		while ((errorNumber = glGetError()) != GL_NO_ERROR) {

			const char* errorMessage;

			switch (errorNumber) {

				case GL_INVALID_ENUM:
					errorMessage = "An invalid enum has been specified.";
					break;

				case GL_INVALID_VALUE:
					errorMessage = "An invalid value has been specified";
					break;

				case GL_INVALID_OPERATION:
					errorMessage = "An invalid operation has been performed.";
					break;

				case GL_STACK_OVERFLOW:
					errorMessage = "Stack overflown.";
					break;

				case GL_STACK_UNDERFLOW:
					errorMessage = "Stack underflown.";
					break;

				case GL_OUT_OF_MEMORY:
					errorMessage = "Out of memory.";
					break;

				case GL_INVALID_FRAMEBUFFER_OPERATION:
					errorMessage = "An invalid framebuffer operation has been performed.";
					break;

				default:
					errorMessage = "An unknown error occured.";
					break;

			}

			error("OpenGL error %d: %s", errorNumber, errorMessage);

		}

	#endif

	}

}

namespace Limits {

	u32 getMaxTextureSize() {
		return maxTextureSize;
	}

	u32 getMax3DTextureSize() {
		return max3DTextureSize;
	}

	u32 getMaxArrayTextureLayers() {
		return maxArrayTextureLayers;
	}

	u32 getMaxMipmapLevels() {
		return maxMipmapLevels;
	}

	float getMaxTextureAnisotropy() {
		return maxAnisotropy;
	}

	u32 getMaxTextureUnits() {
		return maxTextureUnits;
	}

	u32 getMaxDepthSamples() {
		return maxDepthSamples;
	}

	u32 getMaxColorSamples() {
		return maxColorSamples;
	}

	u32 getMaxIntegerSamples() {
		return maxIntegerSamples;
	}

	u32 getMaxColorAttachments() {
		return maxColorAttachments;
	}

	u32 getMaxDrawBuffers() {
		return maxDrawBuffers;
	}

	u32 getMaxUniformBlockBindings() {
		return maxUniformBlockBindings;
	}

}


void setRowUnpackAlignment(Alignment a) {
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1 << static_cast<u32>(a));
}



void setRowPackAlignment(Alignment a) {
	glPixelStorei(GL_PACK_ALIGNMENT, 1 << static_cast<u32>(a));
}


GLE_END