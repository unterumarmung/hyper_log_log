/**
 * @file hll/heplers.hxx
 * @brief A constexpr C++11 implementations of some STL functions
 * @author Daniil Dudkin (unterumarmung)
 */
#ifndef HYPER_LOG_LOG_HELPERS_HXX
#define HYPER_LOG_LOG_HELPERS_HXX

#include <array>
#include <type_traits> // std::is_nothrow_copy_assignable
#include "details.hxx" // HLL_CONSTEXPR_OR_INLINE

namespace hll
{
namespace helpers
{

template<typename T>
HLL_CONSTEXPR_OR_INLINE const T& max(const T& lhs, const T& rhs) noexcept(noexcept(lhs > rhs))
{
    return lhs > rhs ? lhs : rhs;
}

template<typename T, std::size_t N>
HLL_CONSTEXPR_OR_INLINE void
array_fill(std::array<T, N>& array, const T& filler) noexcept(std::is_nothrow_copy_assignable<T>::value)
{
    for (std::size_t i = 0; i < N; ++i)
    {
        array[i] = filler;
    }
}

} // namespace helpers
} // namespace hll

#endif //HYPER_LOG_LOG_HELPERS_HXX
