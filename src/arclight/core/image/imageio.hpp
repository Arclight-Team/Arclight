/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 imageio.hpp
 */

#pragma once

#include "image.hpp"
#include "decode/bitmapdecoder.hpp"
#include "util/bool.hpp"



namespace ImageIO {

	namespace Detail {

		std::vector<u8> loadFile(const Path& path);

	}

	template<Pixel P>
	Image<P> load(const Path& path) {

		std::string ext = path.getExtension();

		if (ext == ".bmp") {
			return load<P, BitmapDecoder>(path);
		} else if (Bool::any(ext, ".jpg", ".jpeg", ".jfif")) {

		}

		throw ImageException("Unknown image file format");

	}

	template<Pixel P, ImageDecoder Decoder, class... Args>
	Image<P> load(const std::span<const u8>& bytes, Args&&... args) {
		return decode<Decoder, Args...>(bytes, std::forward<Args>(args)...).template getImage<P>();
	}

	template<Pixel P, ImageDecoder Decoder, class... Args>
	Image<P> load(const Path& path, Args&&... args) {
		return load<P, Decoder, Args...>(Detail::loadFile(path), std::forward<Args>(args)...);
	}

	template<ImageDecoder Decoder, class... Args>
	Decoder decode(const Path& path, Args&&... args) {
		return decode<Decoder, Args...>(Detail::loadFile(path), std::forward<Args>(args)...);
	}

	template<ImageDecoder Decoder, class... Args>
	Decoder decode(const std::span<const u8>& bytes, Args&&... args) {

		Decoder decoder(std::forward<Args>(args)...);
		decoder.decode(bytes);

		return decoder;

	}

}