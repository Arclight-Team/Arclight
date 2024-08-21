/*
 *	 Copyright (c) 2023 - Arclight Team
 *
 *	 This file is part of Lumina. All rights reserved.
 *
 *	 Version.cpp
 */

#include "Util/Version.hpp"

#include <ranges>



std::string Version::toString(char separator) const {

	std::string s = std::to_string(segments[0]);

	for (SizeT i = 1; i < segments.size(); i++) {
		s += separator + std::to_string(segments[i]);
	}

	return s;

}



bool Version::isSupportedBy(std::string supportedVersions) const {

	if (supportedVersions.empty()) {
		return true;
	}

	std::erase(supportedVersions, ' ');

	bool validVersion = false;

	try {

		for (const auto range : std::views::split(supportedVersions, '|')) {

			std::string_view view(range);

			if (view.empty()) {
				throw VersionParseException(view);
			}

			switch (view[0]) {

				case '@':
					validVersion |= compareAt(Version(view.substr(1)));
					break;

				case '>':
					validVersion |= *this >= Version(view.substr(1));
					break;

				case '<':
					validVersion |= *this < Version(view.substr(1));
					break;

				case '=':
					validVersion |= *this == Version(view.substr(1));
					break;

				default:
				{
					SizeT separatorOffset = view.find('-');

					// Range or no operator
					if (separatorOffset != std::string_view::npos) {

						std::string_view left = view.substr(0, separatorOffset);
						std::string_view right = view.substr(separatorOffset + 1);

						validVersion |= (*this >= Version(left) && *this < Version(right));

					} else {

						// @ - operator
						validVersion |= compareAt(Version(view));

					}

				}

			}

			if (validVersion) {
				break;
			}

		}

	} catch (const VersionParseException& v) {

		return false;

	}

	return validVersion;

}



bool Version::compareAt(const Version& v) const {

	SizeT segmentCount = 1;

	for (SizeT i = segments.size() - 1; i > 0; i--) {

		if (segments[i] != 0) {

			segmentCount = i + 1;
			break;

		}

	}

	if (segmentCount >= v.segments.size()) {

		SizeT min = Math::min(segmentCount, v.segments.size());

		for (SizeT i = 0; i < min; i++) {

			if (segments[i] != v.segments[i]) {
				return false;
			}

		}

		return true;

	}

	return false;

}