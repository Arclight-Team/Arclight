/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 spriteregistry.cpp
 */

#include "spritefactory.hpp"
#include "util/assert.hpp"



bool SpriteFactory::contains(u64 id) const {
	return spriteTypes.contains(id);
}



const SpriteType& SpriteFactory::get(u64 id) const {
	return spriteTypes.at(id);
}




void SpriteFactory::destroy(u64 id) {
	spriteTypes.erase(id);
}



void SpriteFactory::destroyAll() {
	spriteTypes.clear();
}