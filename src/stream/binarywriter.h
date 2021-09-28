#pragma once

#include "util/bits.h"
#include "outputstream.h"
#include "arcconfig.h"


class BinaryWriter {

public:

    BinaryWriter(OutputStream& stream, bool convertEndianess = false, ByteOrder order = ByteOrder::Little) : stream(stream), convert(convertEndianess && Bits::requiresEndianConversion(order)) {}

    template<Arithmetic T>
	void write(T value) {

		using Type = std::remove_cv_t<T>;

		Type in = value;

		if (convert) {
            in = Bits::swap(in);
		}

        auto writtenBytes = stream.write(&in, sizeof(Type));

#ifndef ARC_STREAM_ACCELERATE
		if (writtenBytes != sizeof(Type)) {
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

		using Type = std::remove_cv_t<T>;
        SizeT size = data.size();
        SizeT bytes = sizeof(Type) * size;

		if (convert) {

            for(SizeT i = 0; i < size; i++) {

                Type in = data[i];
                in = Bits::swap(in);
    
                auto writtenBytes = stream.write(&in, sizeof(Type));

#ifndef ARC_STREAM_ACCELERATE
                if (writtenBytes != sizeof(Type)) {
                    arc_force_assert("Failed to write data to stream");
                }
#endif

            }

		} else {

            auto writtenBytes = stream.write(data.data(), bytes);

#ifndef ARC_STREAM_ACCELERATE
            if (writtenBytes != bytes) {
                arc_force_assert("Failed to write data to stream");
            }
#endif

        }

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