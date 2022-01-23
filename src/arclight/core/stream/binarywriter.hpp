/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 binarywriter.hpp
 */

#pragma once

#include "outputstream.hpp"
#include "util/bits.hpp"
#include "util/typetraits.hpp"
#include "arcconfig.hpp"


class BinaryWriter {

public:

    BinaryWriter(OutputStream& stream, bool convertEndianess = false, ByteOrder order = ByteOrder::Little) : stream(stream), convert(convertEndianess && Bits::requiresEndianConversion(order)) {}

    template<Arithmetic T>
	void write(T value) noexcept {

		T in = value;

		if (convert) {
            in = Bits::swap(in);
		}

	    [[maybe_unused]] SizeT writtenBytes = stream.write(&in, sizeof(T));

#ifndef ARC_STREAM_ACCELERATE
		if (writtenBytes != sizeof(T)) {
			arc_force_assert("Failed to write data to stream");
		}
#endif

	}

    template<Arithmetic T>
	void write(const std::span<const T>& data, SizeT count) {
        write(data.subspan(0, count));
	}

	template<Arithmetic T>
	void write(const std::span<const T>& data) {

        SizeT size = data.size();
        SizeT bytes = sizeof(T) * size;

		if (convert) {

            for(SizeT i = 0; i < size; i++) {

                T in = data[i];
                in = Bits::swap(in);
    
	            [[maybe_unused]] SizeT writtenBytes = stream.write(&in, sizeof(T));

#ifndef ARC_STREAM_ACCELERATE
                if (writtenBytes != sizeof(T)) {
                    arc_force_assert("Failed to write data to stream");
                }
#endif

            }

		} else {

			[[maybe_unused]] SizeT writtenBytes = stream.write(data.data(), bytes);

#ifndef ARC_STREAM_ACCELERATE
            if (writtenBytes != bytes) {
                arc_force_assert("Failed to write data to stream");
            }
#endif

        }

	}


	void skip(u64 n) {
		stream.seek(n);
	}


	u64 getPosition() const {
		return stream.getPosition();
	}

    OutputStream& getStream() {
        return stream;
    }

    const OutputStream& getStream() const {
        return stream;
    }

private:

    OutputStream& stream;
    const bool convert;

};