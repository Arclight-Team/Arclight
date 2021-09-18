#pragma once

#include "types.h"
#include <vector>


class InputStream;

namespace LZ77 {

    std::vector<u8> compress(InputStream& stream);
    std::vector<u8> decompress(InputStream& stream);

}