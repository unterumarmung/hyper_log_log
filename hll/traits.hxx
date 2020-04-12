/**
 * @file hll/traits.hxx
 * @brief Utility type traits
 * @author Daniil Dudkin (unterumarmung)
 */
#ifndef HLL_TRAITS_HXX
#define HLL_TRAITS_HXX

#include <utility>

namespace hll
{
namespace traits
{

/**
 * A type trait to identify does the type have data member function
 */
template<typename, typename>
struct has_data_member_function;

/**
 * A type trait to identify does the type have size member function
 */
template<typename, typename>
struct has_size_member_function;

/// void_t implementation to use in C++11
template<typename...> using void_t = void;

template<typename T, typename = void>
struct has_data_member_function : std::false_type
{
};

template<typename T>
struct has_data_member_function<T,
        void_t<decltype(std::declval<T>().data())>>
        : std::true_type
{
};

template<typename T, typename = void>
struct has_size_member_function : std::false_type
{
};

template<typename T>
struct has_size_member_function<T,
        void_t<decltype(std::declval<T>().size())>>
        : std::true_type
{
};

template<typename T, typename Base = typename std::conditional<
        has_data_member_function<T>::value    // has T::data
        && has_size_member_function<T>::value    // has T::size
        && std::is_fundamental<typename T::value_type>::value, // value_type is fundamental
        std::true_type,
        std::false_type>
::type>
struct is_ra_fundamental_container : Base
{
};


} // namespace traits
} // namespace hll

#endif //HLL_TRAITS_HXX
