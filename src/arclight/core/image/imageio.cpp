/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 imageio.cpp
 */

#include "imageio.hpp"
#include "filesystem/file.hpp"



std::vector<u8> ImageIO::Detail::loadFile(const Path& path) {

	File file(path);

	if (!file.open()) {
		throw ImageException("Failed to open file " + path.toString());
	}

	std::vector<u8> data = file.readAll();
	file.close();

	return data;

}



void ImageIO::Detail::saveFile(const Path& path, std::span<const u8> data) {

	File file(path, File::Out);

	if (!file.open()) {
		throw ImageException("Failed to open file " + path.toString());
	}

	file.write(data);
	file.close();

}
