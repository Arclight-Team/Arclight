#pragma once

#include "util/bits.h"
#include "types.h"

#include <span>



namespace Crypto {

    struct MDConstruction {

        u8 prevLastBlock[64];
        u8 lastBlock[64];
        SizeT blocks;
        bool prevBlockUsed;

    };

    MDConstruction mdConstruct(const std::span<const u8>& data, ByteOrder order) noexcept;

}