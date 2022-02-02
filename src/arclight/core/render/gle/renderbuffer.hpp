/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 renderbuffer.hpp
 */

#pragma once

#include "imageformat.hpp"
#include "globject.hpp"

GLE_BEGIN


class Renderbuffer : public GLObject {

public:

	virtual bool create() override;
	void bind();
	virtual void destroy() override;

	bool isBound() const;
	bool isInitialized() const;

	void setStorage(u32 w, u32 h, ImageFormat format, u32 samples = 0);

	u32 getWidth() const;
	u32 getHeight() const;
	u32 getSamples() const;
	ImageFormat getImageFormat() const;

private:

	friend class Framebuffer;

	u32 width;
	u32 height;
	u32 samples;
	ImageFormat format;

	static inline u32 boundRenderbufferID = invalidBoundID;

};


GLE_END