/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 SharedBuffer.hpp
 */

#pragma once

#include "SyncBuffer.hpp"



class SharedBuffer {

public:

	void create();

	void setProjection(const Mat4f& proj);

	void update();
	void bind();

private:

	constexpr static u32 bufferSize = sizeof(Mat4f);

	std::shared_ptr<class SharedBufferData> data;

	SyncBuffer buffer;

};