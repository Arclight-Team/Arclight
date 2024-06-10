/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 CTGroupTableBuffer.hpp
 */

#pragma once

#include "SyncBuffer.hpp"

#include <memory>



class CTGroupTableBuffer {

public:

    void create();

    void setTableSize(u32 size);
    void setSlot(u32 ctID, u32 slot);

    void update();
    void bind();

    u32 getTableSize() const;

private:

    std::shared_ptr<class CTGroupData> data;
    SyncBuffer buffer;

};