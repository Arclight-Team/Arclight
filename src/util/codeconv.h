#pragma once

#include <string>


namespace CodeConv {

    //Standard Shift-JIS to UTF-32
    std::u32string shiftJISToUTF32(const std::string_view& sjisString);

    //Standard Big5 to UTF-32
    std::u32string big5ToUTF32(const std::string_view& big5String);

    //Wansung with UHC extensions to UTF-32
    std::u32string wansungToUTF32(const std::string_view& wansungString);

    //Johab to UTF-32
    std::u32string johabToUTF32(const std::string_view& johabString);

}