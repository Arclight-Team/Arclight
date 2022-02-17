/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 ctgroupnode.hpp
 */

#pragma once

#include "types.hpp"

#include <unordered_map>



class CTGroupNode {

public:

    void addCTReference(u32 ctID);
    void deleteCTReference(u32 ctID);

    void clear();

private:

    std::unordered_map<u32, u32> ctRefCounts;

};