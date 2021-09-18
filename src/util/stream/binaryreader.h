#pragma once

#include "util/bits.h"
#include "inputstream.h"
#include "util/assert.h"


class BinaryReader {

public:

    BinaryReader(InputStream& stream, bool convertEndianess = false, ByteOrder order = ByteOrder::Little) : stream(stream), convert(convertEndianess && Bits::requiresEndianConversion(order)) {}

    template<Arithmetic T>
	T read() {

		using Type = std::remove_cv_t<T>;

		Type in;

		if (stream.read(&in, sizeof(Type)) != sizeof(Type)) {
			arc_force_assert("Failed to read data from stream");
            return {};
		}

		if (convert) {

			if constexpr (sizeof(Type) == 2) { in = static_cast<Type>(arc_swap16(static_cast<u16>(in))); }
			else if constexpr (sizeof(Type) == 4) { in = static_cast<Type>(arc_swap32(static_cast<u32>(in))); }
			else if constexpr (sizeof(Type) == 8) { in = static_cast<Type>(arc_swap64(static_cast<u64>(in))); }
            else { /* Don't convert here */ }

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

                if (stream.read(&in, sizeof(T)) != sizeof(T)) {
			        arc_force_assert("Failed to read data from stream");
                    return;
                }

                if constexpr (sizeof(T) == 2) { in = static_cast<T>(arc_swap16(static_cast<u16>(in))); }
                else if constexpr (sizeof(T) == 4) { in = static_cast<T>(arc_swap32(static_cast<u32>(in))); }
                else if constexpr (sizeof(T) == 8) { in = static_cast<T>(arc_swap64(static_cast<u64>(in))); }

            }

		} else {

            if (stream.read(data.data(), bytes) != bytes) {
			    arc_force_assert("Failed to read data from stream");
                return;
            }

        }

	}

private:

    InputStream& stream;
    const bool convert;

};