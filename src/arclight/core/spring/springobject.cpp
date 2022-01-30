/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 springobject.cpp
 */

#include "springobject.hpp"



void SpringObject::markUpdated() {
	flags |= 0x1;
}



void SpringObject::clearUpdated() {
	flags &= 0xFE;
}