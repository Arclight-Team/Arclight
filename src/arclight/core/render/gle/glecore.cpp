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

namespace __Detail {

	u32 openglContextVersion = 0;
	bool openglDebugContext = false;

	u32 maxTextureSize = 0;
	u32 max3DTextureSize = 0;
	u32 maxArrayTextureLayers = 0;
	u32 maxMipmapLevels = 0;
	float maxAnisotropy = 1.0;

	u32 maxVertexTextureUnits = 0;
	u32 maxFragmentTextureUnits = 0;
	u32 maxGeometryTextureUnits = 0;
	u32 maxTessCtrlTextureUnits = 0;
	u32 maxTessEvalTextureUnits = 0;
	u32 maxComputeTextureUnits = 0;
	u32 maxCombinedTextureUnits = 0;

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

		__Detail::openglContextVersion = majorVersion << 8 | minorVersion;
		__Detail::openglDebugContext = flags & GL_CONTEXT_FLAG_DEBUG_BIT;

		GLE::info("OpenGL %d.%d context set up", majorVersion, minorVersion);
		GLE::info("Debug context %s", (__Detail::openglDebugContext ? "enabled" : "disabled"));

		i32 tmp = 0;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &tmp);
		__Detail::maxTextureSize = tmp;
		glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &tmp);
		__Detail::max3DTextureSize = tmp;
		glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &tmp);
		__Detail::maxArrayTextureLayers = tmp;

		u32 texSize = __Detail::maxTextureSize;
		__Detail::maxMipmapLevels = 0;

		while (texSize != 1) {
			texSize /= 2;
			__Detail::maxMipmapLevels++;
		}

		if (GLE_EXT_SUPPORTED(ARB_texture_filter_anisotropic)) {
			glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &__Detail::maxAnisotropy);
		}

		glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &tmp);
		__Detail::maxVertexTextureUnits = tmp;
		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &tmp);
		__Detail::maxFragmentTextureUnits = tmp;
		glGetIntegerv(GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS, &tmp);
		__Detail::maxGeometryTextureUnits = tmp;
		glGetIntegerv(GL_MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS, &tmp);
		__Detail::maxTessCtrlTextureUnits = tmp;
		glGetIntegerv(GL_MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS, &tmp);
		__Detail::maxTessEvalTextureUnits = tmp;
		glGetIntegerv(GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS, &tmp);
		__Detail::maxComputeTextureUnits = tmp;
		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &tmp);
		__Detail::maxCombinedTextureUnits = tmp;

		glGetIntegerv(GL_MAX_DEPTH_TEXTURE_SAMPLES, &tmp);
		__Detail::maxDepthSamples = tmp;
		glGetIntegerv(GL_MAX_COLOR_TEXTURE_SAMPLES, &tmp);
		__Detail::maxColorSamples = tmp;
		glGetIntegerv(GL_MAX_INTEGER_SAMPLES, &tmp);
		__Detail::maxIntegerSamples = tmp;

		glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &tmp);
		__Detail::maxColorAttachments = tmp;
		glGetIntegerv(GL_MAX_DRAW_BUFFERS, &tmp);
		__Detail::maxDrawBuffers = tmp;

		glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &tmp);
		__Detail::maxUniformBlockBindings = tmp;

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
		return __Detail::maxTextureSize;
	}

	u32 getMax3DTextureSize() {
		return __Detail::max3DTextureSize;
	}

	u32 getMaxArrayTextureLayers() {
		return __Detail::maxArrayTextureLayers;
	}

	u32 getMaxMipmapLevels() {
		return __Detail::maxMipmapLevels;
	}

	float getMaxTextureAnisotropy() {
		return __Detail::maxAnisotropy;
	}

	u32 getMaxVertexTextureUnits() {
		return __Detail::maxVertexTextureUnits;
	}

	u32 getMaxFragmentTextureUnits() {
		return __Detail::maxFragmentTextureUnits;
	}

	u32 getMaxGeometryTextureUnits() {
		return __Detail::maxGeometryTextureUnits;
	}

	u32 getMaxTessControlTextureUnits() {
		return __Detail::maxTessCtrlTextureUnits;
	}

	u32 getMaxTessEvaluationTextureUnits() {
		return __Detail::maxTessEvalTextureUnits;
	}

	u32 getMaxComputeTextureUnits() {
		return __Detail::maxComputeTextureUnits;
	}

	u32 getMaxCombinedTextureUnits() {
		return __Detail::maxCombinedTextureUnits;
	}

	u32 getMaxDepthSamples() {
		return __Detail::maxDepthSamples;
	}

	u32 getMaxColorSamples() {
		return __Detail::maxColorSamples;
	}

	u32 getMaxIntegerSamples() {
		return __Detail::maxIntegerSamples;
	}

	u32 getMaxColorAttachments() {
		return __Detail::maxColorAttachments;
	}

	u32 getMaxDrawBuffers() {
		return __Detail::maxDrawBuffers;
	}

	u32 getMaxUniformBlockBindings() {
		return __Detail::maxUniformBlockBindings;
	}

}


void setRowUnpackAlignment(Alignment a) {
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1 << static_cast<u32>(a));
}



void setRowPackAlignment(Alignment a) {
	glPixelStorei(GL_PACK_ALIGNMENT, 1 << static_cast<u32>(a));
}


GLE_END