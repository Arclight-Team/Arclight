#include "common.h"
#include "math/math.h"

#include <algorithm>


namespace Crypto {

    void mdConstruct(MDConstruction& construct, const std::span<const u8>& data, ByteOrder order) noexcept {

        //Lengths
        u64 bytes = data.size();
        u64 bits = bytes * 8;

        //Helper variables
        u32 rem = bytes % 64;
        SizeT lbStart = bytes / 64 * 64;

        //Whether prevLastBlock will contain the second last block
        construct.prevBlockUsed = rem > 55;
        
        //Total block count
        construct.blocks = Math::alignUp(bytes, 64) / 64 + construct.prevBlockUsed;

        //Fill last blocks
        if(construct.prevBlockUsed) {

            std::copy_n(data.data() + lbStart, rem, construct.prevLastBlock);
            construct.prevLastBlock[rem] = 0x80;
            
            std::fill(construct.prevLastBlock + rem + 1, construct.prevLastBlock + 64, u8(0));
            std::fill_n(construct.lastBlock, 56, 0);

        } else {

            std::copy_n(data.data() + lbStart, rem, construct.lastBlock);
            construct.lastBlock[rem] = 0x80;

            std::fill(construct.lastBlock + rem + 1, construct.lastBlock + 56, u8(0));

        }

        //Conditionally convert
        if (Bits::requiresEndianConversion(order)) {
            bits = Bits::swap(bits);
        }

        //Add length
        Bits::disassemble(bits, construct.lastBlock + 56);

    }

}