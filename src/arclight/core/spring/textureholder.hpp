/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 textureholder.hpp
 */

#pragma once

#include "compositetexture.hpp"
#include "util/uri.hpp"
#include "types.hpp"

#include <unordered_map>



class TextureHolder {

	struct PreloadEntry {
		Uri path;
		u32 group;
	};

public:

	enum class CompositorMode {
		Fast,
		Optimal,
		Add
	};


	TextureHolder();

	void allowTextureComposite(CompositeTexture::Type type);
	void denyTextureComposite(CompositeTexture::Type type);

	void addPreload(u64 id, const Uri& path, u32 compositeGroup = -1);
	void removePreload(u64 id);
	void clearPreloads();

	void loadAndComposite(CompositorMode mode);

private:

	CompositeTexture::Type allowedTypes;
	std::unordered_map<u64, PreloadEntry> preloads;
	std::unordered_map<u64, CompositeTexture> textures;

};