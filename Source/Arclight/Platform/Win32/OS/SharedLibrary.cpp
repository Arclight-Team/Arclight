/*
 *	 Copyright (c) 2023 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 SharedLibrary.cpp
 */

#include "SharedLibrary.hpp"
#include "Common.hpp"
#include "Util/Log.hpp"
#include "Common/Assert.hpp"
#include "Common/Win32.hpp"



OS::SharedLibrary::SharedLibrary() : handle(nullptr) {}

OS::SharedLibrary::SharedLibrary(const std::string& name) : OS::SharedLibrary::SharedLibrary() { open(name); }




bool OS::SharedLibrary::open(const std::string& name) {

	if (loaded()) {

		LogE("OS") << "Close the previous shared library first before attempting to open " << name;
		return false;

	}

	HMODULE module = GetModuleHandleW(OS::String::toUTF16(name).c_str());

	if (module) {

		handle = module;
		libName = name;

	}

	return loaded();

}



void OS::SharedLibrary::close() {

	if (loaded()) {

		handle = nullptr;
		libName.clear();

	}

}



void* OS::SharedLibrary::getRawSymbol(std::string symbol) {

	arc_assert(loaded(), "Library not loaded");

	return reinterpret_cast<void*>(GetProcAddress(static_cast<HMODULE>(handle), symbol.data()));

}



bool OS::SharedLibrary::loaded() const {
	return handle != nullptr;
}



std::string OS::SharedLibrary::name() const {
	return libName;
}



OS::SharedLibrary OS::loadLibrary(const std::string& name, bool loadOnce) {

	if (loadOnce && GetModuleHandleW(OS::String::toUTF16(name).c_str()) != nullptr) {
		return SharedLibrary(name);
	}

	HMODULE module = LoadLibraryExW(OS::String::toUTF16(name).c_str(), nullptr, 0);

	if (!module) {
		LogE("OS") << "Failed to load library " << name;
		return {};
	}

	SharedLibrary lib;
	lib.libName = name;
	lib.handle = module;

	return lib;

}

void OS::unloadLibrary(const std::string& name) {

	OS::SharedLibrary lib(name);

	if (!lib.loaded()) {
		LogE("OS") << "Library " << name << " already unloaded";
	}

	if (!FreeLibrary(static_cast<HMODULE>(lib.handle))) {
		LogE("OS") << "Failed to unload " << name;
	}

}