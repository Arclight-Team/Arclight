/*
*	 Copyright (c) 2024 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 PackageManager.hpp
 */

#pragma once

#if __has_include("Lumina/Lumina.hpp")
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


using PackageInitializer = std::function<bool()>;
using PackageVoid = bool;


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

	void registerInitializer(const std::string& pkgName, const PackageInitializer& init) {
		registerOrInvokeInitializers(pkgName, init, false);
	}

	bool invokeInitializers() {
		return registerOrInvokeInitializers("", {}, true);
	}

private:

	static bool registerOrInvokeInitializers(const std::string& pkgName, const PackageInitializer& init, bool invoke) {

		static std::unordered_map<std::string, PackageInitializer> initializers;
		static bool initialized = false;

		if (initialized) {
			return true;
		}

		if (invoke) {

#if __has_include("Lumina/Lumina.hpp")

			for (SizeT i : __LuminaInitOrder) {

				std::string pkgName(Packages[i].name);

				if (initializers.contains(pkgName)) {

					if (!initializers[pkgName]()) {
						return false;
					}

				}

			}

#endif

			initialized = true;

		} else {

			initializers.try_emplace(pkgName, init);

		}

		return true;

	}

	constexpr static Package NullPackage;

#if __has_include("Lumina/Lumina.hpp")

	constexpr static SizeT PackageCount = __LuminaPkgNames.size();

	constexpr static std::array<Package, PackageCount> Packages = []() {

		std::array<Package, PackageCount> packages;

		for (SizeT i : Range(PackageCount)) {

			packages[i].name = __LuminaPkgNames[i];
			packages[i].version = __LuminaPkgVersions[i];
			packages[i].variant = __LuminaPkgVariants[i];

		}

		return packages;

	}();

#else

	constexpr static std::array<Package, 0> Packages;

#endif

};
