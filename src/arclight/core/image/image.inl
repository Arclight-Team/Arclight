/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 image.inl
 */

#include "util/assert.hpp"



template<Pixel P>
constexpr Image<P>::Image() : Image(0, 0) {}

template<Pixel P>
constexpr Image<P>::Image(u32 width, u32 height, const PixelType& pixel) : width(width), height(height), pixels(std::make_unique<PixelType[]>(width * height)) {
	std::fill_n(pixels.get(), width * height, pixel);
}

template<Pixel P>
constexpr Image<P>::Image(u32 width, u32 height, const std::span<const u8>& sourceData) : width(width), height(height), pixels(std::make_unique<PixelType[]>(width * height)) {
	setRawData(sourceData);
}

template<Pixel P>
constexpr Image<P>::Image(const Image<P>& image) : width(image.getWidth()), height(image.getHeight()), pixels(std::make_unique<PixelType[]>(width * height)) {
	std::copy_n(image.pixels.get(), pixelCount(), pixels.get());
}

template<Pixel P>
constexpr Image<P>& Image<P>::operator=(const Image<P>& image) {

	if (*this != image) {

		if (pixelCount() != image.pixelCount()) {
			pixels = std::make_unique<PixelType[]>(image.pixelCount());
		}

		width = image.getWidth();
		height = image.getHeight();

		std::copy_n(image.pixels.get(), pixelCount(), pixels.get());

	}

	return *this;

}

template<Pixel P>
constexpr void Image<P>::reset() {

	width = 0;
	height = 0;
	pixels.reset();

}

template<Pixel P>
constexpr void Image<P>::clear(const PixelType& clearPixel) {

	for(u32 y = 0; y < height; y++) {

		for(u32 x = 0; x < width; x++) {

			pixels[y * width + x] = clearPixel;

		}

	}

}

template<Pixel P>
constexpr void Image<P>::setRawData(const std::span<const u8>& src, u64 startPixel) {

	arc_assert(startPixel + src.size() / PixelBytes <= pixelCount(), "Cannot copy pixel data to smaller image");

	for(SizeT i = 0; i < src.size() / PixelBytes; i++) {
		pixels[startPixel + i] = PixelType(src.subspan(i * PixelBytes, PixelBytes));
	}

}


template<Pixel P>
constexpr u32 Image<P>::getWidth() const {
	return width;
}

template<Pixel P>
constexpr u32 Image<P>::getHeight() const {
	return height;
}

template<Pixel P>
constexpr SizeT Image<P>::pixelCount() const {
	return width * height;
}

template<Pixel P>
constexpr std::span<typename Image<P>::PixelType> Image<P>::getImageBuffer() {
	return std::span{pixels.get(), pixelCount()};
}

template<Pixel P>
constexpr std::span<const typename Image<P>::PixelType> Image<P>::getImageBuffer() const {
	return std::span{pixels.get(), pixelCount()};
}

template<Pixel P>
constexpr void Image<P>::setPixel(u32 x, u32 y, const PixelType& pixel) {

	arc_assert(x < width && y < height, "Pixel access out of bounds");
	pixels[y * width + x] = pixel;

}

template<Pixel P>
constexpr const typename Image<P>::PixelType& Image<P>::getPixel(u32 x, u32 y) const {

	arc_assert(x < width && y < height, "Pixel access out of bounds");
	return pixels[y * width + x];

}

template<Pixel P>
constexpr typename Image<P>::PixelType& Image<P>::getPixel(u32 x, u32 y) {

	arc_assert(x < width && y < height, "Pixel access out of bounds");
	return pixels[y * width + x];

}

template<Pixel P>
template<class Filter, class... Args>
void Image<P>::applyFilter(Args&&... args) {
	Filter::run(*this, std::forward<Args>(args)...);
}

template<Pixel P>
constexpr void Image<P>::resize(ImageScaling scaling, u32 w, u32 h) {

	if(!w) {
		Log::error("Image", "Cannot resize image to a width of 0");
		return;
	}


	if(!h) {

		//Take the aspect ratio
		float aspect = static_cast<float>(width) / height;
		h = static_cast<u32>(Math::round(w / aspect));

	}

	//Skip case
	if(w == width && h == height) {
		return;
	}

	std::unique_ptr<PixelType[]> resizedPixelData = std::make_unique<PixelType[]>(w * h);

	switch(scaling) {

		case ImageScaling::Nearest:

			for(u32 y = 0; y < h; y++) {

				u32 cy = static_cast<u32>(Math::floor((y + 0.5) * height / h));

				for(u32 x = 0; x < w; x++) {

					u32 cx = static_cast<u32>(Math::floor((x + 0.5) * width / w));
					resizedPixelData[y * w + x] = getPixel(cx, cy);

				}

			}

			break;

		case ImageScaling::Bilinear:

			for(u32 y = 0; y < h; y++) {

				float fy = (y + 0.5f) * height / h;
				float ty = fy - static_cast<u32>(fy);

				u32 cy0, cy1;

				if(ty >= 0.5) {
					cy0 = static_cast<u32>(fy);
					cy1 = Math::min(cy0 + 1, height - 1);
				} else {
					cy1 = static_cast<u32>(fy);
					cy0 = cy1 ? cy1 - 1 : 0;
					ty += 1;
				}

				float dy = ty - 0.5f;

				for(u32 x = 0; x < w; x++) {

					float fx = (x + 0.5f) * width / w;
					float tx = fx - static_cast<u32>(fx);
					u32 cx0, cx1;

					if(tx >= 0.5) {
						cx0 = static_cast<u32>(fx);
						cx1 = Math::min(cx0 + 1, width - 1);
					} else {
						cx1 = static_cast<u32>(fx);
						cx0 = cx1 ? cx1 - 1 : 0;
						tx += 1;
					}

					float dx = tx - 0.5f;

					const PixelType& p00 = getPixel(cx0, cy0);
					const PixelType& p01 = getPixel(cx0, cy1);
					const PixelType& p10 = getPixel(cx1, cy0);
					const PixelType& p11 = getPixel(cx1, cy1);

					//No need to check for max pixel values since those are impossible to reach by standard interpolation
					Vec4f v00(p00.getRed(), p00.getGreen(), p00.getBlue(), p00.getAlpha());
					Vec4f v01(p01.getRed(), p01.getGreen(), p01.getBlue(), p01.getAlpha());
					Vec4f v10(p10.getRed(), p10.getGreen(), p10.getBlue(), p10.getAlpha());
					Vec4f v11(p11.getRed(), p11.getGreen(), p11.getBlue(), p11.getAlpha());

					Vec4f a0 = (1.0f - dx) * v00 + dx * v10;
					Vec4f a1 = (1.0f - dx) * v01 + dx * v11;
					Vec4f a = (1.0f - dy) * a0 + dy * a1;

					PixelType p;
#ifdef ARC_PIXEL_EXACT
					p.setRGBA(static_cast<u32>(Math::round(a.x)), static_cast<u32>(Math::round(a.y)), static_cast<u32>(Math::round(a.z)), static_cast<u32>(Math::round(a.w)));
#else
					p.setRGBA(static_cast<u32>(a.x), static_cast<u32>(a.y), static_cast<u32>(a.z), static_cast<u32>(a.w));
#endif
					resizedPixelData[y * w + x] = p;

				}

			}

			break;

		default:
			arc_force_assert("Illegal scaling parameter");
			break;

	}

	width = w;
	height = h;
	pixels = resizedPixelData;

}

template<Pixel P>
constexpr void Image<P>::flipY() {

	PixelType* p = pixels.get();

	for(u32 i = 0; i < height / 2; i++) {
		std::swap_ranges(p + width * i, p + width * (i + 1), p + width * (height - i - 1));
	}

}

template<Pixel P>
constexpr void Image<P>::copy(Image<P>& destImage, const RectUI& src, const Vec2ui& dest) {

	if (this == &destImage) {

		copy(src, dest);
		return;

	}

	for (u32 y = 0; y < src.getHeight(); y++) {

		for (u32 x = 0; x < src.getWidth(); x++) {

			destImage.setPixel(dest.x + x, dest.y + y, this->getPixel(src.getX() + x, src.getY() + y));

		}

	}

}

template<Pixel P>
constexpr void Image<P>::copy(const RectUI& src, const Vec2ui& dest) {

	u32 xStart = 0;
	u32 xEnd = 0;

	u32 yStart = 0;
	u32 yEnd = 0;

	if (src.getPosition() == dest) {

		return;

	} else if (src.getX() + src.getY() * this->getWidth() > dest.x + dest.y * this->getWidth()) {

		xEnd = src.getWidth();
		yEnd = src.getHeight();

	} else {

		xStart = src.getWidth();
		yStart = src.getHeight();

	}

	for (u32 y = yStart; y != yEnd; y += (yStart < yEnd ? 1 : -1)) {

		for (u32 x = xStart; x != xEnd; x += (xStart < xEnd ? 1 : -1)) {

			x += (xStart < xEnd ? 0 : -1);
			y += (yStart < yEnd ? 0 : -1);

			this->setPixel(dest.x + x, dest.y + y, this->getPixel(src.getX() + x, src.getY() + y));

			x -= (xStart < xEnd ? 0 : -1);
			y -= (yStart < yEnd ? 0 : -1);

		}

	}

}


template<Pixel P>
template<Pixel Q>
Image<Q> Image<P>::convert() const {

	if constexpr (P == Q) {
		return *this;
	}

	Image<Q> img(width, height);

	for (u32 y = 0; y < height; y++) {

		for (u32 x = 0; x < width; x++) {

			img.setPixel(x, y, PixelConverter::convert<Q>(getPixel(x, y)));

		}

	}

	return img;

}


template<Pixel P>
RawImage Image<P>::makeRaw() {

	u32 oldWidth = width;
	u32 oldHeight = height;
	PixelType* releasedPtr = pixels.release();

	reset();

	return RawImage(oldWidth, oldHeight, releasedPtr);

}


template<Pixel P>
Image<P> Image<P>::fromRaw(RawImage& image, bool allowConversion) {

	u32 w = image.getWidth();
	u32 h = image.getHeight();
	Pixel pixelFormat = image.getFormat();

	auto createImage = [&]() -> Image<P> {

		Image<P> newImage;
		newImage.width = w;
		newImage.height = h;
		newImage.pixels = std::unique_ptr<PixelType[]>(reinterpret_cast<PixelType*>(image.release().data()));

		return newImage;

	};

	auto convertImage = [&]<Pixel From>() -> Image<P> {

		using Type = ::PixelType<From>;

		Image<From> newImage;
		newImage.width = w;
		newImage.height = h;
		newImage.pixels = std::unique_ptr<Type[]>(reinterpret_cast<Type*>(image.release().data()));

		return newImage.template convert<P>();

	};

	if (allowConversion) {

		if (pixelFormat == P) {
			return createImage();
		}

		switch (image.getFormat()) {

			case Pixel::Grayscale8: return convertImage.template operator()<Pixel::Grayscale8>();
			case Pixel::BGR5:       return convertImage.template operator()<Pixel::BGR5>();
			case Pixel::RGB5:       return convertImage.template operator()<Pixel::RGB5>();
			case Pixel::BGR8:       return convertImage.template operator()<Pixel::BGR8>();
			case Pixel::RGB8:       return convertImage.template operator()<Pixel::RGB8>();
			case Pixel::RGBA8:      return convertImage.template operator()<Pixel::RGBA8>();
			case Pixel::ABGR8:      return convertImage.template operator()<Pixel::ABGR8>();
			case Pixel::BGRA8:      return convertImage.template operator()<Pixel::BGRA8>();
			case Pixel::ARGB8:      return convertImage.template operator()<Pixel::ARGB8>();
			default: ARC_UNREACHABLE;

		}

	} else {

		if (image.getFormat() != P) {
			throw ImageException("Bad image cast");
		}

		return createImage();

	}

}