/**
 * @file hll/heplers.hxx
 * @brief A constexpr C++11 implementations of some STL functions
 * @author Daniil Dudkin (unterumarmung)
 */
#ifndef HYPER_LOG_LOG_HELPERS_HXX
#define HYPER_LOG_LOG_HELPERS_HXX

namespace hll { namespace helpers {

template <typename T>
constexpr const T& max(const T& lhs, const T& rhs) noexcept(noexcept(lhs > rhs))
{
    return lhs > rhs ? lhs : rhs;
}

} // namespace helpers
} // namespace hll

#endif //HYPER_LOG_LOG_HELPERS_HXX
