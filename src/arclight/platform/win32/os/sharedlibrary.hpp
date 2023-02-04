/*
 *	 Copyright (c) 2023 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 sharedlibrary.hpp
 */

#pragma once

#include "common/concepts.hpp"

#include <string>



namespace OS {

	class SharedLibrary {

	public:

		SharedLibrary();
		SharedLibrary(const std::string& name);

		bool open(const std::string& name);
		void close();

		template<CC::PointerType F>
		F getSymbol(const std::string& symbol) {
			return reinterpret_cast<F>(getRawSymbol(symbol));
		}

		bool loaded() const;
		std::string name() const;

	private:

		friend SharedLibrary loadLibrary(const std::string&, bool);
		friend void unloadLibrary(const std::string&);

		void* getRawSymbol(std::string symbol);

		std::string libName;
		void* handle;

	};

	// Shared functions
	SharedLibrary loadLibrary(const std::string& name, bool loadOnce = true);
	void unloadLibrary(const std::string& name);

}