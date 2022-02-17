/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 ctgrouplink.cpp
 */

#include "ctgrouplink.hpp"



void CTGroupLink::addCTReference(u32 ctID) {
    ++ctRefCounts[ctID];
}



void CTGroupLink::deleteCTReference(u32 ctID) {
    
    u32 refs = --ctRefCounts[ctID];

    if (!refs) {
        ctRefCounts.erase(ctID);
    }

}



void CTGroupLink::clear() {
    ctRefCounts.clear();
}