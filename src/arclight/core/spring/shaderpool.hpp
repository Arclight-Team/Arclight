/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 shaderpool.hpp
 */

#pragma once

#include "filesystem/path.hpp"
#include "types.hpp"

#include <unordered_map>



class ShaderHandle;

class ShaderPool {

public:

	ShaderPool() = default;

	std::shared_ptr<ShaderHandle> get(const Path& vsPath, const Path& fsPath);

	void clear();
	void clearUnused();

private:

	struct Key {
		Path vsPath;
		Path fsPath;
	};

	struct Hash {
		SizeT operator()(const Key& key) const;
	};

	struct Predicate {
		bool operator()(const Key& a, const Key& b) const;
	};

	std::unordered_map<Key, std::shared_ptr<ShaderHandle>, Hash, Predicate> pool;

};