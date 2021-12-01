#pragma once

#include "types.h"
#include "util/bits.h"
#include "util/string.h"
#include "util/concepts.h"
#include "util/typetraits.h"

#include <array>
#include <string>



template<SizeT Size>
class Hash {

    using StorageT = u8;
    constexpr static SizeT Segments = Size / (sizeof(StorageT) * 8);

    static_assert(Size % (sizeof(StorageT) * 8) == 0, "Size must be a multiple of the underlying storage type");

public:

    constexpr Hash() noexcept {

        for(SizeT i = 0; i < Segments; i++) {
            segments[i] = 0;
        }

    }

    template<Integer... J>
    constexpr Hash(J... j) noexcept {
        set(j...);
    }

    template<Integer... J> requires ((sizeof(J) + ...) * 8 == Size && ((sizeof(J) >= sizeof(StorageT)) && ...))
    constexpr void set(J... j) {

        SizeT i = 0;
        ((Bits::disassemble(j, segments + i), i += sizeof(J) / sizeof(StorageT)), ...);

    }

    constexpr bool operator==(const Hash<Size>& other) noexcept {

        bool equal = true;

        for(SizeT i = 0; i < Segments; i++) {
            equal &= segments[i] == other.segments[i];
        }

        return equal;

    }

    constexpr std::array<u8, Size / 8> toArray() const noexcept {

        std::array<u8, Size / 8> a;

        for(SizeT i = 0; i < Segments; i++) {
            Bits::disassemble(segments[i], a.data() + i * sizeof(StorageT));
        }

        return a;

    }

    constexpr std::string toString(bool upper = false) const noexcept {

        std::string s;

        for(SizeT i = 0; i < Segments; i++) {
            s += String::toHexString(Bits::swap(segments[i]), upper, false);
        }

        return s;

    }

private:

    StorageT segments[Segments];

};