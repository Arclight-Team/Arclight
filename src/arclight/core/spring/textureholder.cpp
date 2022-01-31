/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 textureholder.cpp
 */

#include "textureholder.hpp"


TextureHolder::TextureHolder() : allowedTypes(static_cast<CompositeTexture::Type>(0)) {}



void TextureHolder::allowTextureComposite(CompositeTexture::Type type) {
	allowedTypes |= type;
}



void TextureHolder::denyTextureComposite(CompositeTexture::Type type) {
	allowedTypes &= ~type;
}



void TextureHolder::addPreload(u64 id, const Uri& path, u32 compositeGroup) {

	if (textures.contains(id)) {
		return;
	}

	preloads[id] = PreloadEntry {path, compositeGroup};

}



void TextureHolder::removePreload(u64 id) {
	preloads.erase(id);
}



void TextureHolder::clearPreloads() {
	preloads.clear();
}



void TextureHolder::loadAndComposite(CompositorMode mode) {

}