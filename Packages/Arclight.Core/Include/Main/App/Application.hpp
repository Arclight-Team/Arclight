/*
 *	 Copyright (c) 2024 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 Application.hpp
 */

#pragma once

#include "App/Configuration.hpp"
#include "App/PackageManager.hpp"
#include "Filesystem/File.hpp"



namespace Application {

	namespace Private {

		inline PackageManager pkgManager;
		inline Configuration appConfig;

	}

	constexpr std::string_view getName() {

	#ifdef ARCLIGHT_APP_NAME
		return ARCLIGHT_APP_NAME;
	#endif
		return "";

	}

	constexpr std::string_view getVersion() {

	#ifdef ARCLIGHT_APP_VERSION
		return ARCLIGHT_APP_VERSION;
	#endif
		return "";

	}

	constexpr std::string_view getAuthor() {

	#ifdef ARCLIGHT_APP_AUTHOR
		return ARCLIGHT_APP_AUTHOR;
	#endif
		return "";

	}

	constexpr std::string_view getCompilerName() {

	#ifdef ARCLIGHT_APP_COMPILER_NAME
		return ARCLIGHT_APP_COMPILER_NAME;
	#endif
		return "";

	}

	constexpr std::string_view getBuildDate() {

	#ifdef ARCLIGHT_APP_BUILD_DATE
		return ARCLIGHT_APP_BUILD_DATE;
	#endif
		return "";

	}

	constexpr std::string_view getBuildConfiguration() {

	#ifdef ARCLIGHT_APP_BUILD_CONFIG
		return ARCLIGHT_APP_BUILD_CONFIG;
	#endif
		return "";

	}

	constexpr std::string_view getPlatformName() {

	#ifdef ARCLIGHT_APP_PLATFORM
		return ARCLIGHT_APP_PLATFORM;
	#endif
		return "";

	}

	constexpr const PackageManager& getPackageManager() {
		return Private::pkgManager;
	}

	template<class T>
	constexpr T getConfigurationValue(const std::string& name, const T& defaultValue = T()) {
		return Private::appConfig.get<T>(name, defaultValue);
	}

	template<class T>
	constexpr void setConfigurationValue(const std::string& name, const T& value) {
		Private::appConfig.set<T>(name, value);
	}

	bool initialize();
	void finalize();

	Path getExecutablePath();
	Path getExecutableDirectory();
	Path getCurrentWorkingDirectory();

	void setCurrentWorkingDirectory(const Path& path);

};