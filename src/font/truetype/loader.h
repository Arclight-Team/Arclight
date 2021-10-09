#pragma once

#include "truetype.h"


class InputStream;

namespace TrueType {

    Font loadFont(InputStream& stream);

}