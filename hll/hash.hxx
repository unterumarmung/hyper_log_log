/**
 * @file hll/hash.hxx
 * @brief This file contains hash-function wrappers of murmurhash3
 * @author Daniil Dudkin (unterumarmung)
 */
#ifndef HLL_HASH_HXX
#define HLL_HASH_HXX

#include <type_traits>

#include "murmur_hash.hxx"
#include "traits.hxx"

namespace hll
{
    /// type alias for hash functions return-type
    using hash_result = uint32_t;

    /**
     * Hashes the fundamental types
     * @tparam T the value type
     * @param value the value
     * @return hash
     */
    template <typename T, typename std::enable_if<std::is_fundamental<T>::value>::type* = nullptr>
    constexpr hash_result hash(const T& value) noexcept
    {
        return murmur_hash(&value, sizeof(T), /*seed = */ 0);
    }

    /**
     * Hashes "random-access" containers of the fundamental types
     * @tparam T the container type, must have T::size and T::data member functions and T::value_type member type
     * @param value the container
     * @return hash
     */
    template <typename T, typename std::enable_if<hll::traits::is_ra_fundamental_container <T>::value>::type* = nullptr>
    constexpr hash_result hash(const T& value)
            noexcept(noexcept(value.data()) && noexcept(value.size()))
    {
        return murmur_hash(value.data(), value.size() * sizeof(T::value_type), /*seed = */ 0);
    }

} //namespace hll


#endif //HLL_HASH_HXX
