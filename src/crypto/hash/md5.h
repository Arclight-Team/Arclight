#pragma once

#include "types.h"
#include "crypto/hash/hash.h"

#include <span>



namespace MD5 {

    Hash<128> hash(const std::span<const u8>& data);

}