/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 qoidecoder.hpp
 */

#pragma once

#include "decoder.hpp"
#include "bitmap.hpp"
#include "image/image.hpp"
#include "stream/binaryreader.hpp"



class QOIDecoder : public IImageDecoder {

public:

    QOIDecoder() : validDecode(false) {}

    void decode(std::span<const u8> data);

    template<Pixel P>
    Image<P> getImage() {

        if (!validDecode) {
            throw ImageDecoderException("Bad image decode");
        }

        return Image<P>::fromRaw(image);

    }

private:

    BinaryReader reader;
    RawImage image;
    bool validDecode;

};