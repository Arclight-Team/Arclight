/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 shaderpool.cpp
 */

#include "shaderpool.hpp"
#include "shaderhandle.hpp"
#include "render/utility/shaderloader.hpp"
#include "util/log.hpp"

#include <memory>



std::shared_ptr<ShaderHandle> ShaderPool::get(const Path& vsPath, const Path& fsPath) {

	try {

		Key key {vsPath, fsPath};

		auto it = pool.find(key);

		if (it == pool.end()) {

			std::shared_ptr<ShaderHandle> handle = std::make_shared<ShaderHandle>(ShaderLoader::fromFiles(vsPath, fsPath));
			pool[key] = handle;

			return handle;

		} else {

			return it->second;

		}

	} catch (const std::exception& e) {
		LogE("Spring").print("Failed to obtain shader instance: %s", e.what());
	}

	return nullptr;

}



void ShaderPool::clear() {
	pool.clear();
}



void ShaderPool::clearUnused() {

	for (auto it = pool.begin(); it != pool.end();) {

		if (it->second.use_count() == 1) {
			it = pool.erase(it);
		} else {
			it++;
		}

	}

}



SizeT ShaderPool::Hash::operator()(const Key& key) const {
	return std::hash<std::string>{}(key.vsPath.getCanonical().toNativeString() + "///" + key.fsPath.getCanonical().toNativeString());
}



bool ShaderPool::Predicate::operator()(const Key& a, const Key& b) const {
	return a.vsPath.getCanonical().toNativeString() == b.vsPath.getCanonical().toNativeString() && a.fsPath.getCanonical().toNativeString() == b.fsPath.getCanonical().toNativeString();
}