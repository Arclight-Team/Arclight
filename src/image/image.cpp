#include "image.h"
#include "math/math.h"


/*
Image::Image() {
    setData(0, 0, PixelType::RGB8, {});
}

Image::Image(u32 width, u32 height, PixelType type, const std::span<u8>& source) {
    setData(width, height, type, source);
}



void Image::setData(const std::span<u8>& source) {

    u64 size = getDataSize();

    data.resize(size);
    auto it = std::copy(source.begin(), source.end(), data.begin());

    if(source.size() < size) {
        std::fill(it, data.end(), getFormat().nullByte());
    }

}


void Image::setData(u32 width, u32 height, PixelType type, const std::span<u8>& source) {

    this->width = width;
    this->height = height;
    this->type = type;
    setData(source);

}


std::span<const u8> Image::getData() const {
    return data;
}



u64 Image::getDataSize() const {
    return width * height * getFormat().bytesPerPixel();
}



const PixelFormat& Image::getFormat() const {
    return formatList[static_cast<SizeT>(type)];
}
*/