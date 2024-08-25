/*
 *	 Copyright (c) 2023 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 Version.hpp
 */

#pragma once

#include "Common/Exception.hpp"
#include "Common/Types.hpp"
#include "Meta/Concepts.hpp"
#include "Math/Math.hpp"

#include <vector>



class VersionParseException : public ArclightException {

public:

	explicit VersionParseException(std::string_view str) noexcept : ArclightException("Failed to parse version string " + std::string(str)) {}

	virtual const char* name() const noexcept override { return "Version Parse Exception"; }

};


class Version {

public:

	constexpr Version() : segments {0} {}

	template<CC::Integer... V>
	constexpr explicit Version(V... v) : segments {static_cast<u32>(v)...} {}

	template<CC::Integer I>
	constexpr explicit Version(const std::vector<I>& v) : segments(v.begin(), v.end()) {}

	constexpr explicit Version(const std::string_view& s) {
		fromString(s);
	}

	constexpr void normalize() {

		SizeT newSize = segments.size();

		for (SizeT i = segments.size(); i > 0; i--) {

			if (segments[i] == 0) {
				newSize--;
			} else {
				break;
			}

		}

		segments.resize(newSize);

	}

	constexpr u32 getMajorVersion() const {
		return segments[0];
	}

	constexpr u32 getMinorVersion() const {
		return segments.size() > 1 ? segments[1] : 0;
	}

	constexpr u32 getPatchVersion() const {
		return segments.size() > 2 ? segments[2] : 0;
	}

	constexpr u32 getNthVersion(SizeT index) const {
		return segments.size() > index ? segments[index] : 0;
	}

	constexpr void setMajorVersion(u32 major) {
		segments[0] = major;
	}

	constexpr void setMinorVersion(u32 minor) {

		if (segments.size() == 1) {
			segments.emplace_back(minor);
		} else {
			segments[1] = minor;
		}

	}

	constexpr void setPatchVersion(u32 patch) {

		switch (segments.size()) {

			case 1:
				segments.resize(3);
				segments[2] = patch;
				break;

			case 2:
				segments.emplace_back(patch);
				break;

			default:
				segments[2] = patch;
				break;

		}

	}

	constexpr void setNthVersion(SizeT index, u32 version) {

		if (index > 127) {
			arc_force_assert("Illegal version index; may not exceed 127\n");
			return;
		}

		if (index >= segments.size()) {
			segments.resize(index + 1);
		}

		segments[index] = version;

	}

	std::string toString(char separator = '.') const;

	constexpr void fromString(const std::string_view& versionString) {

		u32 version = 0;
		SizeT segmentStart = 0;
		std::vector<u32> newSegments;

		for (SizeT i = 0; i < versionString.size(); i++) {

			char c = versionString[i];

			if (c == '.') {

				if (segmentStart == i) {
					// Double '.'
					throw VersionParseException(versionString);
				}

				if (versionString[segmentStart] == '0' && (segmentStart + 1) != i) {
					// Version segment starts with 0 and is not a single 0
					throw VersionParseException(versionString);
				}

				std::from_chars(versionString.data() + segmentStart, versionString.data() + i, version);
				newSegments.emplace_back(version);
				segmentStart = i + 1;

			} else if (c < '0' || c > '9') {

				// Illegal character
				throw VersionParseException(versionString);

			}

		}

		if (versionString.size() == segmentStart) {
			// Ends in . or version string is empty
			throw VersionParseException(versionString);
		}

		std::from_chars(versionString.data() + segmentStart, versionString.data() + versionString.size(), version);
		newSegments.emplace_back(version);

		segments = newSegments;

	}

	bool isSupportedBy(std::string supportedVersions) const;

	constexpr auto operator<=>(const Version& other) const {

	    SizeT maximum = Math::max(segments.size(), other.segments.size());

	    for (SizeT i = 0; i < maximum; i++) {

			u32 a = getNthVersion(i);
			u32 b = other.getNthVersion(i);

	        if (a != b) {
	            return a <=> b;
	        }

	    }

	    return std::strong_ordering::equal;

	};

	constexpr bool operator==(const Version& other) const {
		return (*this <=> other) == std::strong_ordering::equal;
	}

private:

	bool compareAt(const Version& v) const;

	std::vector<u32> segments;

};