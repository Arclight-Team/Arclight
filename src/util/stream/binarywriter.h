#pragma once

#include "util/bits.h"
#include "outputstream.h"


class BinaryWriter {

public:

    BinaryWriter(OutputStream& stream, bool convertEndianess = false, ByteOrder order = ByteOrder::Little) : stream(stream), convert(convertEndianess && Bits::requiresEndianConversion(order)) {}

    template<Arithmetic T>
	void write(T value) {

		using Type = std::remove_cv_t<T>;

		Type in = value;

		if (convert) {

			if constexpr (sizeof(Type) == 2) { in = static_cast<Type>(arc_swap16(static_cast<u16>(in))) }
			else if constexpr (sizeof(Type) == 4) { in = static_cast<Type>(arc_swap32(static_cast<u32>(in))) }
			else if constexpr (sizeof(Type) == 8) { in = static_cast<Type>(arc_swap64(static_cast<u64>(in))) }
            else { /* Don't convert here */ }

		}

		if (stream.write(&in, sizeof(Type)) != sizeof(Type)) {
			arc_force_assert("Failed to write data to stream");
		}

	}

    template<Arithmetic T>
	void write(const std::span<T>& data, SizeT count) {
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

                if constexpr (sizeof(Type) == 2) { in = static_cast<Type>(arc_swap16(static_cast<u16>(in))) }
                else if constexpr (sizeof(Type) == 4) { in = static_cast<Type>(arc_swap32(static_cast<u32>(in))) }
                else if constexpr (sizeof(Type) == 8) { in = static_cast<Type>(arc_swap64(static_cast<u64>(in))) }
    
                if (stream.write(&in, sizeof(Type)) != sizeof(Type)) {
                    arc_force_assert("Failed to write data to stream");
                }

            }

		} else {

            if (stream.write(data.data(), bytes) != bytes) {
                arc_force_assert("Failed to write data to stream");
            }

        }

	}

private:

    OutputStream& stream;
    const bool convert;

};