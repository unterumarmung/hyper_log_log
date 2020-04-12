/**
 * @file hll/murmur_hash.hxx
 * @brief MurmurHash3 C++ implementation
 * @author Daniil Dudkin (unterumarmung)
 */

#ifndef HLL_MURMUR_HASH_HXX
#define HLL_MURMUR_HASH_HXX

#include <cstdint>
#include "details.hxx"

/**
 * MurmurHash3 C++ implementation
 * @param key data pointer
 * @param length data length
 * @param seed
 * @return hash
 */
HLL_CONSTEXPR_OR_INLINE uint32_t murmur_hash(const void* key, uint32_t length, uint32_t seed) noexcept
{
    constexpr uint32_t c1 = 0xcc9e2d51;
    constexpr uint32_t c2 = 0x1b873593;
    constexpr uint32_t r1 = 15;
    constexpr uint32_t r2 = 13;
    constexpr uint32_t m = 5;
    constexpr uint32_t n = 0xe6546b64;
    const auto chunk_length = length / 4u;
    const auto chunks = static_cast<const uint32_t*>(key); // 32 bit extract from `key'
    const auto tail = static_cast<const uint8_t*>(key) + chunk_length * 4; // tail - last 8 bytes
    uint32_t h = 0;
    uint32_t k = 0;

    h = seed;

    // for each 4 byte chunk of `key'
    for (auto i = 0u; i < chunk_length; ++i)
    {
        // next 4 byte chunk of `key'
        k = chunks[i];

        // encode next 4 byte chunk of `key'
        k *= c1;
        k = (k << r1) | (k >> (32 - r1));
        k *= c2;

        // append to hash
        h ^= k;
        h = (h << r2) | (h >> (32 - r2));
        h = h * m + n;
    }

    k = 0;

    // remainder
    switch (length & 3u)
    { // `length % 4'
        case 3:
            k ^= (tail[2] << 16u);
        case 2:
            k ^= (tail[1] << 8u);

        case 1:
            k ^= tail[0];
            k *= c1;
            k = (k << r1) | (k >> (32 - r1));
            k *= c2;
            h ^= k;
    }

    h ^= length;

    h ^= (h >> 16u);
    h *= 0x85ebca6b;
    h ^= (h >> 13u);
    h *= 0xc2b2ae35;
    h ^= (h >> 16u);

    return h;
}

#endif // HLL_MURMUR_HASH_HXX
