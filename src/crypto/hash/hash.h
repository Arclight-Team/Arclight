#pragma once

#include "types.h"
#include "util/bits.h"
#include "util/concepts.h"
#include "util/typetraits.h"

#include <string>
#include <array>
#include <algorithm>



template<SizeT Size, Integer StorageT = u32> requires (Size % (sizeof(StorageT) * 8) == 0)
class Hash {

    constexpr static SizeT Segments = Size / (sizeof(StorageT) * 8);

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

    template<Integer... J> requires (TT::IsAllSame<J...> && sizeof...(J) * TT::SizeofN<0, J...> * 8 == Size && TT::SizeofN<0, J...> >= sizeof(StorageT))
    constexpr void set(J... j) {

        constexpr SizeT divisor = TT::SizeofN<0, J...> / sizeof(StorageT);

        SizeT i = 0;
        (Bits::disassemble(j, segments + i++ * divisor), ...);

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

private:

    StorageT segments[Segments];

};