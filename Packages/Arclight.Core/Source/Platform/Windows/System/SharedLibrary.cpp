/*
 *	 Copyright (c) 2023 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 SharedLibrary.cpp
 */

#include "System/SharedLibrary.hpp"
#include "System/Common.hpp"
#include "Util/Log.hpp"
#include "Common/Assert.hpp"
#include "Common/Win32.hpp"



System::SharedLibrary::SharedLibrary() : handle(nullptr) {}

System::SharedLibrary::SharedLibrary(const std::string& name) : System::SharedLibrary::SharedLibrary() { open(name); }




bool System::SharedLibrary::open(const std::string& name) {

	if (loaded()) {

		LogE("System") << "Close the previous shared library first before attempting to open " << name;
		return false;

	}

	HMODULE module = GetModuleHandleW(System::String::toUTF16(name).c_str());

	if (module) {

		handle = module;
		libName = name;

	}

	return loaded();

}



void System::SharedLibrary::close() {

	if (loaded()) {

		handle = nullptr;
		libName.clear();

	}

}



void* System::SharedLibrary::getRawSymbol(std::string symbol) {

	arc_assert(loaded(), "Library not loaded");

	return reinterpret_cast<void*>(GetProcAddress(static_cast<HMODULE>(handle), symbol.data()));

}



bool System::SharedLibrary::loaded() const {
	return handle != nullptr;
}



std::string System::SharedLibrary::name() const {
	return libName;
}



System::SharedLibrary System::loadLibrary(const std::string& name, bool loadOnce) {

	if (loadOnce && GetModuleHandleW(System::String::toUTF16(name).c_str()) != nullptr) {
		return SharedLibrary(name);
	}

	HMODULE module = LoadLibraryExW(System::String::toUTF16(name).c_str(), nullptr, 0);

	if (!module) {
		LogE("System") << "Failed to load library " << name;
		return {};
	}

	SharedLibrary lib;
	lib.libName = name;
	lib.handle = module;

	return lib;

}

void System::unloadLibrary(const std::string& name) {

	System::SharedLibrary lib(name);

	if (!lib.loaded()) {
		LogE("System") << "Library " << name << " already unloaded";
	}

	if (!FreeLibrary(static_cast<HMODULE>(lib.handle))) {
		LogE("System") << "Failed to unload " << name;
	}

}