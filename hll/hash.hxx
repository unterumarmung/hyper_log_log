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

    hash_result hash(...);

    /**
     * Hashes the fundamental types
     * @tparam T the value type
     * @param value the value
     * @return hash
     */
    template <typename T>
    hash_result hash(const T& value, typename std::enable_if<std::is_fundamental<T>::value>::type* = nullptr)
    {
        auto hash_result = murmur_hash(&value, sizeof(T), /*seed = */ 0);
        return hash_result;
    }

    /**
     * Hashes "random-access" containers of the fundamental types
     * @tparam T the container type, must have T::size and T::data member functions and  T::value_type member type
     * @param value the container
     * @return hash
     */
    template <typename T>
    hash_result hash(const T& value,
            typename std::enable_if<
               hll::traits::has_data_member_function<T>::value    // has T::data
            && hll::traits::has_size_member_function<T>::value    // has T::size
            && std::is_fundamental<typename T::value_type>::value // value_type is fundamental
            >::type* = nullptr)
    {
        auto hash_result = murmur_hash(value.data(), value.size() * sizeof(T::value_type), /*seed = */ 0);
        return hash_result;
    }

} //namespace hll


#endif //HLL_HASH_HXX
