/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 springobject.cpp
 */

#include "springobject.hpp"



bool SpringObject::isDirty() const noexcept {
	return flags & 0x1;
}



void SpringObject::setDirty() noexcept {
	flags |= 0x1;
}



void SpringObject::clearDirty() noexcept {
	flags &= 0xFE;
}