/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 SpriteRegistry.cpp
 */

#include "Spring/SpriteFactory.hpp"
#include "Common/Assert.hpp"



bool SpriteFactory::contains(u32 id) const {
	return spriteTypes.contains(id);
}



const SpriteType& SpriteFactory::get(u32 id) const {

	auto it = spriteTypes.find(id);
	return it->second;

}




void SpriteFactory::destroy(u32 id) {
	spriteTypes.erase(id);
}



void SpriteFactory::destroyAll() {
	spriteTypes.clear();
}