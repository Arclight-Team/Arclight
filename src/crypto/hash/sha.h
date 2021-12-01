#pragma once

#include "types.h"
#include "hash.h"

#include <span>


namespace SHA0 {

    Hash<160> hash(const std::span<const u8>& data) noexcept;

}

namespace SHA1 {

    Hash<160> hash(const std::span<const u8>& data) noexcept;

}

namespace SHA2 {

    Hash<224> hash224(const std::span<const u8>& data) noexcept;
    Hash<256> hash256(const std::span<const u8>& data) noexcept;
    Hash<384> hash384(const std::span<const u8>& data) noexcept;
    Hash<512> hash512(const std::span<const u8>& data) noexcept;
    Hash<224> hash512t224(const std::span<const u8>& data) noexcept;
    Hash<256> hash512t256(const std::span<const u8>& data) noexcept;

}