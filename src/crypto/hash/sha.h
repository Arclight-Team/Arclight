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