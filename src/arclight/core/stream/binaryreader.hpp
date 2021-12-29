/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 binaryreader.hpp
 */

#pragma once

#include "inputstream.hpp"
#include "util/bits.hpp"
#include "util/assert.hpp"
#include "arcconfig.hpp"


class BinaryReader {

public:

    BinaryReader(InputStream& stream, bool convertEndianess = false, ByteOrder order = ByteOrder::Little) : stream(stream), convert(convertEndianess && Bits::requiresEndianConversion(order)) {}

    template<Arithmetic T>
	T read() {

		using Type = std::remove_cv_t<T>;

		Type in;

        auto readBytes = stream.read(&in, sizeof(Type));

#ifndef ARC_STREAM_ACCELERATE
		if (readBytes != sizeof(Type)) {
			arc_force_assert("Failed to read data from stream");
            return {};
		}
#endif

		if (convert) {
            in = Bits::swap(in);
		}

        return in;

	}

    template<Arithmetic T>
	void read(const std::span<T>& data, SizeT count) {
        read(data.subspan(0, count));
	}

	template<Arithmetic T>
	void read(const std::span<T>& data) {

        SizeT size = data.size();
        SizeT bytes = sizeof(T) * size;

		if (convert) {

            for(SizeT i = 0; i < size; i++) {

                T& in = data[i];
                auto readBytes = stream.read(&in, sizeof(T));

#ifndef ARC_STREAM_ACCELERATE
                if (readBytes != sizeof(T)) {
                    arc_force_assert("Failed to read data from stream");
                    return;
                }
#endif

                in = Bits::swap(in);

            }

		} else {

            auto readBytes = stream.read(data.data(), bytes);

#ifndef ARC_STREAM_ACCELERATE
            if (readBytes != bytes) {
			    arc_force_assert("Failed to read data from stream");
                return;
            }
#endif

        }

	}

    InputStream& getStream() {
        return stream;
    }

    const InputStream& getStream() const {
        return stream;
    }

private:

    InputStream& stream;
    const bool convert;

};