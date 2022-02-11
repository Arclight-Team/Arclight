/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 renderbuffer.cpp
 */

#include "renderbuffer.hpp"

#include GLE_HEADER


GLE_BEGIN



bool Renderbuffer::create() {

	if (!isCreated()) {

		glGenRenderbuffers(1, &id);

		if (!id) {

			id = invalidID;
			return false;

		}

	}

	return true;

}



void Renderbuffer::bind() {

	gle_assert(isCreated(), "Renderbuffer hasn't been created yet");

	if (!isBound()) {
		glBindRenderbuffer(GL_RENDERBUFFER, id);
		boundRenderbufferID = id;
	}

}



void Renderbuffer::destroy() {

	if (isCreated()) {

		if (isBound()) {
			boundRenderbufferID = invalidBoundID;
		}

		glDeleteRenderbuffers(1, &id);

		id = invalidID;
		width = 0;
		height = 0;
		samples = 0;
		format = ImageFormat::None;

	}

}



void Renderbuffer::setStorage(u32 w, u32 h, ImageFormat format, u32 samples) { 
	
	gle_assert(isBound(), "Renderbuffer %d has not been bound (attempted to set storage)", id);

	if (samples) {

		if (samples > Image::getMaxSamples(format)) {
			error("Renderbuffer with given format cannot hold more than %d samples (got %d)", Image::getMaxSamples(format), samples);
			return;
		}

	}

	width = w;
	height = h;
	this->samples = samples;
	this->format = format;

	glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, static_cast<u32>(format), w, h);

}



u32 Renderbuffer::getWidth() const {
	return width;
}



u32 Renderbuffer::getHeight() const {
	return height;
}



u32 Renderbuffer::getSamples() const {
	return samples;
}



ImageFormat Renderbuffer::getImageFormat() const {
	return format;
}



bool Renderbuffer::isBound() const {
	return id == boundRenderbufferID;
}



bool Renderbuffer::isInitialized() const {
	return format != ImageFormat::None;
}


GLE_END