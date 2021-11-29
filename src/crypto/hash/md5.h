#pragma once

#include "types.h"
#include "hash.h"

#include <span>



namespace MD5 {

    Hash<128> hash(const std::span<const u8>& data) noexcept;

}