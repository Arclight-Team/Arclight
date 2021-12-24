#pragma once

#include "truetype.hpp"


class InputStream;

namespace TrueType {

    Font loadFont(InputStream& stream);
    std::vector<Font> loadFontCollection(InputStream& stream);

}