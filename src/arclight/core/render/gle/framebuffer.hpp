/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 framebuffer.hpp
 */

#pragma once

#include "globject.hpp"


GLE_BEGIN


class Texture;
class Renderbuffer;

class Framebuffer : public GLObject {

public:

	enum AttachmentIndex {
		DepthIndex			= 0x8D00, // GL_DEPTH_ATTACHMENT
		StencilIndex		= 0x8D20, // GL_STENCIL_ATTACHMENT
		DepthStencilIndex	= 0x821A, // GL_DEPTH_STENCIL_ATTACHMENT
		ColorIndex			= 0x8CE0, // GL_COLOR_ATTACHMENT0
	};

	static inline u32 defaultFramebufferID = 0;

	virtual bool create() override;
	void bind();
	virtual void destroy() override;

	bool isBound() const;
	bool validate() const;

	void attachTexture(u32 attachmentIndex, const Texture& texture);
	void attachTexture(u32 attachmentIndex, const Texture& texture, u32 layer);
	void attachRenderbuffer(u32 attachmentIndex, const Renderbuffer& renderbuffer);

	static void bindDefault();
	static void setViewport(u32 w, u32 h);
	static void setViewport(u32 x, u32 y, u32 w, u32 h);

private:

	static bool validAttachmentIndex(u32 index);

	static inline u32 boundFramebufferID = defaultFramebufferID;

};


GLE_END