/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 ctgrouptablebuffer.cpp
 */

#include "ctgrouptablebuffer.hpp"
#include "render/gle/gle.hpp"



struct CTGroupData {

    CTGroupData() {

        tableUBO.create();
        tableUBO.bind();
        tableUBO.allocate(512, GLE::BufferAccess::StaticDraw);

    }

    ~CTGroupData() {
        tableUBO.destroy();
    }

    GLE::UniformBuffer tableUBO;

};



CTGroupTableBuffer::CTGroupTableBuffer() {
    data = std::make_shared<CTGroupData>();
}



void CTGroupTableBuffer::setTableSize(u32 size) {
    buffer.resize(size);
}



void CTGroupTableBuffer::setSlot(u32 ctID, u32 slot) {

    arc_assert(slot < 16, "Invalid CT slot ID");
    arc_assert(ctID < getTableSize(), "Invalid CT ID");

    SizeT offset = ctID / 2;
    u8 b = buffer[offset];

    b = Bits::clear(ctID, ctID & 1, 4);
    b |= slot << (4 * (ctID & 1));

    buffer.write(offset, b);

}



void CTGroupTableBuffer::update() {

	if (!buffer.hasChanged()) {
		return;
	}

	GLE::UniformBuffer& ubo = data->tableUBO;

	ubo.bind();

	if (ubo.getSize() < buffer.size()) {

		ubo.allocate(Math::alignUp(buffer.size(), 512), GLE::BufferAccess::StaticDraw);
		ubo.update(0, buffer.size(), buffer.data());

	} else {

		ubo.update(buffer.getUpdateStart(), buffer.getUpdateSize(), buffer.updateStartData());

	}

	buffer.finishUpdate();

}



void CTGroupTableBuffer::bind() {

    GLE::UniformBuffer& ubo = data->tableUBO;
    ubo.bindRange(0, 0, buffer.size());

}



u32 CTGroupTableBuffer::getTableSize() const {
    return buffer.size() * 2;
}