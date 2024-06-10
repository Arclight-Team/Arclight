/*
 *	 Copyright (c) 2023 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 Hardware.hpp
 */

#pragma once

#include "Common/Types.hpp"

#include <string>



namespace System::Hardware {

	struct Version {

		constexpr Version() noexcept : Version(0, 0, 0) {}
		constexpr Version(u32 major, u32 minor, u32 patch = 0) noexcept : major(major), minor(minor), patch(patch) {}

		constexpr bool operator==(const Version& v) const noexcept = default;

		constexpr auto operator<=>(const Version& v) const noexcept {

			if (major == v.major) {

				if (minor == v.minor) {
					return patch <=> v.patch;
				}

				return minor <=> v.minor;

			}

			return major <=> v.major;

		}

		std::string toString() const {

			std::string s = std::to_string(major);

			if (patch || minor) {

				s.append(".");
				s.append(std::to_string(minor));

				if (patch) {

					s.append(".");
					s.append(std::to_string(patch));

				}

			}

			return s;

		}

		u32 major;
		u32 minor;
		u32 patch;

	};

	std::string getComputerName();
	bool setComputerName(const std::string& name);

	Version getOSVersion();

	SizeT getHardwareConcurrency();

}