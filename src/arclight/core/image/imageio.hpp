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
#include "decode/jpegdecoder.hpp"
#include "decode/ppmdecoder.hpp"
#include "decode/qoidecoder.hpp"
#include "util/bool.hpp"



namespace ImageIO {

	namespace Detail {

		std::vector<u8> loadFile(const Path& path);

	}



	/*
	 *  Raw decoder functions
	 */
	template<CC::ImageDecoder Decoder, class... Args>
	Decoder decode(const std::span<const u8>& bytes, std::optional<Pixel> reqFormat, Args&&... args) {

		Decoder decoder(reqFormat, std::forward<Args>(args)...);
		decoder.decode(bytes);

		return decoder;

	}

	template<CC::ImageDecoder Decoder, class... Args>
	Decoder decode(const Path& path, std::optional<Pixel> reqFormat = {}, Args&&... args) {
		return decode<Decoder, Args...>(Detail::loadFile(path), reqFormat, std::forward<Args>(args)...);
	}



	/*
	 *  Forced format functions
	 */
	template<Pixel P, CC::ImageDecoder Decoder, class... Args>
	Image<P> load(const std::span<const u8>& bytes, Args&&... args) {
		return Image<P>::fromRaw(decode<Decoder, Args...>(bytes, P, std::forward<Args>(args)...).getImage(), true);
	}

	template<Pixel P, CC::ImageDecoder Decoder, class... Args>
	Image<P> load(const Path& path, Args&&... args) {
		return load<P, Decoder, Args...>(Detail::loadFile(path), std::forward<Args>(args)...);
	}



	/*
	 *  Generic format functions
	 */
	template<CC::ImageDecoder Decoder, class... Args>
	RawImage load(const std::span<const u8>& bytes, Args&&... args) {
		return decode<Decoder, Args...>(bytes, {}, std::forward<Args>(args)...).getImage();
	}

	template<CC::ImageDecoder Decoder, class... Args>
	RawImage load(const Path& path, Args&&... args) {
		return load<Decoder, Args...>(Detail::loadFile(path), std::forward<Args>(args)...);
	}



	namespace Detail {

		template<bool Raw, Pixel P = Pixel::RGB8>
		auto fileLoad(const Path& path) -> TT::Conditional<Raw, RawImage, Image<P>> {

			auto doLoad = []<CC::ImageDecoder Decoder>(const Path& p) {

				if constexpr (Raw) {
					return load<Decoder>(p);
				} else {
					return load<P, Decoder>(p);
				}

			};

			std::string ext = path.getExtension();

			if (ext == ".bmp") {
				return doLoad.template operator()<BitmapDecoder>(path);
			} else if (Bool::any(ext, ".jpg", ".jpeg", ".jfif")) {
				return doLoad.template operator()<JPEGDecoder>(path);
			} else if (ext == ".ppm") {
	            return doLoad.template operator()<PPMDecoder>(path);
	        } else if (ext == ".qoi") {
	            return doLoad.template operator()<QOIDecoder>(path);
	        }

			throw ImageException("Unknown image file format");

		}

	}



	/*
	 *  Auto-detecting format functions
	 */
	template<Pixel P>
	Image<P> load(const Path& path) {
		return Detail::fileLoad<false, P>(path);
	}

	inline RawImage load(const Path& path) {
		return Detail::fileLoad<true>(path);
	}

}