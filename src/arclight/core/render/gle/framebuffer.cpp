/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 framebuffer.cpp
 */

#include "framebuffer.hpp"

#include "texture.hpp"
#include "renderbuffer.hpp"
#include GLE_HEADER


GLE_BEGIN


bool Framebuffer::create() {

	if (!isCreated()) {

		glGenFramebuffers(1, &id);

		if (!id) {

			id = invalidID;
			return false;

		}

	}

	return true;

}



void Framebuffer::bind() {

	gle_assert(isCreated(), "Framebuffer hasn't been created yet");

	if (!isBound()) {
		glBindFramebuffer(GL_FRAMEBUFFER, id);
		boundFramebufferID = id;
	}

}



void Framebuffer::destroy() {

	if (isCreated()) {

		if (isBound()) {
			//For framebuffers, we revert to the defaulf framebuffer ID (as specified by OpenGL)
			boundFramebufferID = defaultFramebufferID;
		}

		glDeleteFramebuffers(1, &id);

		id = invalidID;

	}

}



bool Framebuffer::isBound() const {
	return id == boundFramebufferID;
}



bool Framebuffer::validate() const {

	gle_assert(isBound(), "Framebuffer %d has not been bound (attempted to validate)", id);

	u32 state = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	switch (state) {

		case GL_FRAMEBUFFER_COMPLETE:
			info("Framebuffer %d has been successfully validated", id);
			return true;

		case GL_FRAMEBUFFER_UNDEFINED:
			error("Framebuffer %d does not exist", id);
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			error("Framebuffer %d contains an incomplete attachment", id);
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			error("Framebuffer %d does not have at least one color attachment", id);
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
			error("Framebuffer %d has incomplete draw buffers", id);
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
			error("Framebuffer %d has incomplete read buffers", id);
			break;

		case GL_FRAMEBUFFER_UNSUPPORTED:
			error("Framebuffer %d has unsupported render format", id);
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
			error("Framebuffer %d has inconsistent multisample settings", id);
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
			error("Framebuffer %d has inconsistent layer settings", id);
			break;

		default:			
			error("Unknown error occurred in framebuffer %d", id);
			break;

	}

	return false;

}



void Framebuffer::attachTexture(u32 attachmentIndex, const Texture& texture) {

	gle_assert(isBound(), "Framebuffer %d has not been bound (attempted to attach texture)", id);
	gle_assert(texture.isInitialized(), "Texture %d has not been initialized (attempted to attach texture)", id);
	gle_assert(validAttachmentIndex(attachmentIndex), "Framebuffer attachment index 0x%04X is invalid (id = %d)", attachmentIndex, id);
	gle_assert(texture.getTextureType() == TextureType::Texture1D ||
			   texture.getTextureType() == TextureType::Texture2D ||
			   texture.getTextureType() == TextureType::MultisampleTexture2D, "Cannot attach layered texture to non-layered framebuffer attachment");

	glFramebufferTexture(GL_FRAMEBUFFER, static_cast<u32>(attachmentIndex), texture.id, 0);

}



void Framebuffer::attachTexture(u32 attachmentIndex, const Texture& texture, u32 layer) {

	gle_assert(isBound(), "Framebuffer %d has not been bound (attempted to attach texture)", id);
	gle_assert(texture.isInitialized(), "Texture %d has not been initialized (attempted to attach texture)", id);
	gle_assert(validAttachmentIndex(attachmentIndex), "Framebuffer attachment index 0x%04X is invalid (id = %d)", attachmentIndex, id);
	gle_assert(texture.getTextureType() != TextureType::Texture1D &&
			   texture.getTextureType() != TextureType::Texture2D &&
			   texture.getTextureType() != TextureType::MultisampleTexture2D, "Cannot attach non-layered texture to layered framebuffer attachment");

	switch (texture.getTextureType()) {

		case TextureType::Texture3D:
		case TextureType::ArrayTexture2D:
		case TextureType::MultisampleArrayTexture2D:

			if (layer >= texture.getDepth()) {
				error("Layer %d exceeds texture layer count of %d", layer, texture.getDepth());
				return;
			}

			glFramebufferTextureLayer(GL_FRAMEBUFFER, static_cast<u32>(attachmentIndex), texture.id, 0, layer);
			break;

		case TextureType::ArrayTexture1D:

			if (layer >= texture.getHeight()) {
				error("Layer %d exceeds texture layer count of %d", layer, texture.getHeight());
				return;
			}

			glFramebufferTextureLayer(GL_FRAMEBUFFER, static_cast<u32>(attachmentIndex), texture.id, 0, layer);
			break;

		case TextureType::CubemapTexture:

			if (layer >= 6) {
				error("Layer %d exceeds cubemap texture face count of 6", layer);
				return;
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, static_cast<u32>(attachmentIndex), static_cast<u32>(Texture::getCubemapFace(layer)), texture.id, 0);
			break;

		default:
			gle_force_assert("Texture type 0x%04X cannot be attached to framebuffer", static_cast<u32>(texture.getTextureType()));
			break;

	}

}



void Framebuffer::attachRenderbuffer(u32 attachmentIndex, const Renderbuffer& renderbuffer) {

	gle_assert(isBound(), "Framebuffer %d has not been bound (attempted to attach renderbuffer)", id);
	gle_assert(renderbuffer.isInitialized(), "Renderbuffer %d has not been initialized (attempted to attach renderbuffer)", id);
	gle_assert(validAttachmentIndex(attachmentIndex), "Framebuffer attachment index 0x%04X is invalid (id = %d)", attachmentIndex, id);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, static_cast<u32>(attachmentIndex), GL_RENDERBUFFER, renderbuffer.id);

}



void Framebuffer::bindDefault() {

	if (boundFramebufferID) {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		boundFramebufferID = 0;
	}

}



void Framebuffer::setViewport(u32 w, u32 h) {
	glViewport(0, 0, w, h);
}



void Framebuffer::setViewport(u32 x, u32 y, u32 w, u32 h) {
	glViewport(x, y, w, h);
}



bool Framebuffer::validAttachmentIndex(u32 index) {
	return (index < Limits::getMaxColorAttachments() + ColorIndex) || (index == DepthIndex) || (index == StencilIndex) || (index == DepthStencilIndex);
}


GLE_END