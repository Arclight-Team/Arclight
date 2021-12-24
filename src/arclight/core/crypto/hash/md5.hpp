#pragma once

#include "hash.hpp"
#include "types.hpp"

#include <span>



namespace MD5 {

    Hash<128> hash(const std::span<const u8>& data) noexcept;

}