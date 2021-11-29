#include "sha.h"
#include "common.h"
#include "util/bits.h"

#include <algorithm>



constexpr static u32 sha256Constants[64] = {
    0x428A2F98, 0x71374491, 0xB5C0FBCF, 0xE9B5DBA5, 0x3956C25B, 0x59F111F1, 0x923F82A4, 0xAB1C5ED5,
    0xD807AA98, 0x12835B01, 0x243185BE, 0x550C7DC3, 0x72BE5D74, 0x80DEB1FE, 0x9BDC06A7, 0xC19BF174,
    0xE49B69C1, 0xEFBE4786, 0x0FC19DC6, 0x240CA1CC, 0x2DE92C6F, 0x4A7484AA, 0x5CB0A9DC, 0x76F988DA,
    0x983E5152, 0xA831C66D, 0xB00327C8, 0xBF597FC7, 0xC6E00BF3, 0xD5A79147, 0x06CA6351, 0x14292967,
    0x27B70A85, 0x2E1B2138, 0x4D2C6DFC, 0x53380D13, 0x650A7354, 0x766A0ABB, 0x81C2C92E, 0x92722C85,
    0xA2BFE8A1, 0xA81A664B, 0xC24B8B70, 0xC76C51A3, 0xD192E819, 0xD6990624, 0xF40E3585, 0x106AA070,
    0x19A4C116, 0x1E376C08, 0x2748774C, 0x34B0BCB5, 0x391C0CB3, 0x4ED8AA4A, 0x5B9CCA4F, 0x682E6FF3,
    0x748F82EE, 0x78A5636F, 0x84C87814, 0x8CC70208, 0x90BEFFFA, 0xA4506CEB, 0xBEF9A3F7, 0xC67178F2
};



template<bool SHA1>
constexpr static void dispatchBlockSHA01(const std::span<const u8>& data, u32& a0, u32& b0, u32& c0, u32& d0, u32& e0) {

    u32 w[80];

    for(u32 i = 0; i < 16; i++) {
        w[i] = Bits::big32(Bits::assemble<u32>(data.data() + i * 4));
    }

    for(u32 i = 16; i < 32; i++) {

        u32 x = w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16];

        //SHA1 adds this left rotation
        if constexpr (SHA1) {
            x = Bits::rol(x, 1);
        }

        w[i] = x;

    }

    //Allows better SIMD optimizations
    for(u32 i = 32; i < 80; i++) {

        u32 x = w[i - 6] ^ w[i - 16] ^ w[i - 28] ^ w[i - 32];

        //SHA1 adds this left rotation
        if constexpr (SHA1) {
            x = Bits::rol(x, 2);
        }

        w[i] = x;

    }

    u32 a = a0;
    u32 b = b0;
    u32 c = c0;
    u32 d = d0;
    u32 e = e0;

    u32 f, g;

    for(u32 i = 0; i < 80; i++) {

        if(i < 20) {

            f = (b & c) | (~b & d);
            g = 0x5A827999;

        } else if (i < 40) {

            f = b ^ c ^ d;
            g = 0x6ED9EBA1;

        } else if (i < 60) {

            f = (b & c) | (b & d) | (c & d);
            g = 0x8F1BBCDC;

        } else {

            f = b ^ c ^ d;
            g = 0xCA62C1D6;

        }

        u32 x = Bits::rol(a, 5) + e + f + g + w[i];
        e = d;
        d = c;
        c = Bits::rol(b, 30);
        b = a;
        a = x;

    }

    a0 += a;
    b0 += b;
    c0 += c;
    d0 += d;
    e0 += e;

}



constexpr static void dispatchBlockSHA256(const std::span<const u8>& data, u32 h0[8]) {

    u32 h[8];
    u32 w[64];

    //Message schedule array population
    for(u32 i = 0; i < 16; i++) {
        w[i] = Bits::big32(Bits::assemble<u32>(data.data() + i * 4));
    }

    for(u32 i = 16; i < 64; i++) {
        w[i] = w[i - 16] + w[i - 7] + (Bits::ror(w[i - 15], 7) ^ Bits::ror(w[i - 15], 18) ^ (w[i - 15] >> 3)) + (Bits::ror(w[i - 2], 17) ^ Bits::ror(w[i - 2], 19) ^ (w[i - 2] >> 10));
    }

    //Copy input state
    std::copy_n(h0, 8, h);

    //Compress block
    for(u32 i = 0; i < 64; i++) {

        u32 a = Bits::ror(h[4], 6) ^ Bits::ror(h[4], 11) ^ Bits::ror(h[4], 25);
        u32 b = (h[4] & h[5]) ^ (~h[4] & h[6]);
        u32 c = a + b + h[7] + w[i] + sha256Constants[i];

        u32 d = Bits::ror(h[0], 2) ^ Bits::ror(h[0], 13) ^ Bits::ror(h[0], 22);
        u32 e = (h[0] & h[1]) ^ (h[0] & h[2]) ^ (h[1] & h[2]);
        u32 f = d + e;

        h[7] = h[6];
        h[6] = h[5];
        h[5] = h[4];
        h[4] = h[3] + c;
        h[3] = h[2];
        h[2] = h[1];
        h[1] = h[0];
        h[0] = c + f;

    }

    //Calculate output state
    for(u32 i = 0; i < 8; i++) {
        h0[i] += h[i];
    }

}



template<bool SHA1>
inline static Hash<160> hashSHA01(const std::span<const u8>& data) noexcept {

    Crypto::MDConstruction construct;
    Crypto::mdConstruct(construct, data, ByteOrder::Big);

    u32 a = 0x67452301;
    u32 b = 0xEFCDAB89;
    u32 c = 0x98BADCFE;
    u32 d = 0x10325476;
    u32 e = 0xC3D2E1F0;

    SizeT blocks = construct.blocks;
    u32 specialBlocks = construct.prevBlockUsed + 1;

    //Hash blocks
    if(blocks >= specialBlocks) {

        for(SizeT i = 0; i < blocks - specialBlocks; i++) {
            dispatchBlockSHA01<SHA1>(data.subspan(i * 64, 64), a, b, c, d, e);
        }

    }

    if(construct.prevBlockUsed) {
        dispatchBlockSHA01<SHA1>({construct.prevLastBlock, 64}, a, b, c, d, e);
    }
    
    dispatchBlockSHA01<SHA1>({construct.lastBlock, 64}, a, b, c, d, e);

    return Hash<160>(Bits::big32(a), Bits::big32(b), Bits::big32(c), Bits::big32(d), Bits::big32(e));

}



Hash<160> SHA0::hash(const std::span<const u8>& data) noexcept {
    return hashSHA01<false>(data);
}



Hash<160> SHA1::hash(const std::span<const u8>& data) noexcept {
    return hashSHA01<true>(data);
}



template<bool SHA256>
inline static auto hash2xx(const std::span<const u8>& data) noexcept {

    Crypto::MDConstruction construct;
    Crypto::mdConstruct(construct, data, ByteOrder::Big);

    u32 h[8];

    if constexpr (SHA256) {

        constexpr u32 s[8] = { 0x6A09E667, 0xBB67AE85, 0x3C6EF372, 0xA54FF53A, 0x510E527F, 0x9B05688C, 0x1F83D9AB, 0x5BE0CD19 };
        std::copy_n(s, 8, h);

    } else {

        constexpr u32 s[8] = { 0xC1059ED8, 0x367CD507, 0x3070DD17, 0xF70E5939, 0xFFC00B31, 0x68581511, 0x64F98FA7, 0xBEFA4FA4 };
        std::copy_n(s, 8, h);

    }

    SizeT blocks = construct.blocks;
    u32 specialBlocks = construct.prevBlockUsed + 1;

    if(blocks >= specialBlocks) {

        for(SizeT i = 0; i < blocks - specialBlocks; i++) {
            dispatchBlockSHA256(data.subspan(i * 64, 64), h);
        }

    }

    if(construct.prevBlockUsed) {
        dispatchBlockSHA256({construct.prevLastBlock, 64}, h);
    }
    
    dispatchBlockSHA256({construct.lastBlock, 64}, h);

    std::for_each_n(h, 8, [](u32& x) {
        x = Bits::big32(x);
    });

    if constexpr (SHA256) {
        return Hash<256>(h[0], h[1], h[2], h[3], h[4], h[5], h[6], h[7]);
    } else {
        return Hash<224>(h[0], h[1], h[2], h[3], h[4], h[5], h[6]);
    }

}



Hash<224> SHA2::hash224(const std::span<const u8>& data) noexcept {
    return hash2xx<false>(data);
}



Hash<256> SHA2::hash256(const std::span<const u8>& data) noexcept {
    return hash2xx<true>(data);
}