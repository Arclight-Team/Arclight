/*
*	 Copyright (c) 2024 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 PackageManager.hpp
 */

#pragma once

#ifdef LUMINA_APP
	#include "Lumina/Lumina.hpp"
#endif

#include "Util/Range.hpp"

#include <span>
#include <array>
#include <string>
#include <functional>
#include <unordered_map>



struct Package {

	std::string_view name;
	std::string_view version;
	std::string_view variant;

	constexpr bool valid() const noexcept {
		return !name.empty() || !version.empty() || !variant.empty();
	}

};



class PackageManager {

public:

	constexpr bool isPackageLoaded(std::string_view pkgName) const {
		return std::ranges::find_if(Packages, [&](const Package& pkg) { return pkg.name == pkgName; }) != std::end(Packages);
	}

	constexpr SizeT getPackageCount() const {
		return Packages.size();
	}

	constexpr std::span<const Package> getPackageList() const {
		return Packages;
	}

	constexpr const Package& getPackage(std::string_view pkgName) const {

		auto it = std::ranges::find_if(Packages, [&](const Package& pkg) { return pkg.name == pkgName; });
		return it == std::end(Packages) ? NullPackage : *it;

	}

private:

	constexpr static Package NullPackage;

#ifdef LUMINA_APP

	constexpr static SizeT PackageCount = __Lumina::PkgNames.size();

	constexpr static std::array<Package, PackageCount> Packages = []() {

		std::array<Package, PackageCount> packages;

		for (SizeT i : Range(PackageCount)) {

			packages[i].name = __Lumina::PkgNames[i];
			packages[i].version = __Lumina::PkgVersions[i];
			packages[i].variant = __Lumina::PkgVariants[i];

		}

		return packages;

	}();

#else

	constexpr static std::array<Package, 0> Packages;

#endif

};
