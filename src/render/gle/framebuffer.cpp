#include "framebuffer.h"

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

	}

}



void Framebuffer::destroy() {

	if (isCreated()) {

		if (isBound()) {
			
		}

		glDeleteFramebuffers(1, &id);

		id = invalidID;

	}

}



bool Framebuffer::isBound() {
	return false;
}



GLE_END