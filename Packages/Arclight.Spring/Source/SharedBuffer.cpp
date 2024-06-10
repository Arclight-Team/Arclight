/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 SharedBuffer.cpp
 */

#include "Spring/SharedBuffer.hpp"
#include "Spring/Spring.hpp"
#include "Render/GLE/GLE.hpp"



struct SharedBufferData {

	SharedBufferData(u32 size) {

		sharedUBO.create();
		sharedUBO.bind();
		sharedUBO.allocate(size, GLE::BufferAccess::DynamicDraw);

	}

	~SharedBufferData() {
		sharedUBO.destroy();
	}

	GLE::UniformBuffer sharedUBO;

};



void SharedBuffer::create() {

	if (!data) {

		data = std::make_shared<SharedBufferData>(bufferSize);
		buffer.resize(bufferSize);

	}

}


void SharedBuffer::setProjection(const Mat4f& proj) {
	buffer.write(0, proj);
}



void SharedBuffer::update() {

	if (!buffer.hasChanged()) {
		return;
	}

	GLE::UniformBuffer& ubo = data->sharedUBO;

	ubo.bind();
	ubo.update(buffer.getUpdateStart(), buffer.getUpdateSize(), buffer.updateStartData());

	buffer.finishUpdate();

}



void SharedBuffer::bind() {

	GLE::UniformBuffer& ubo = data->sharedUBO;
	ubo.bindRange(Spring::sharedBufferBinding, 0, buffer.size());

}