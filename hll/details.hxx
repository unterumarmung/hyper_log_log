#ifndef HLL_DETAILS_HXX
#define HLL_DETAILS_HXX

#include <type_traits>

namespace hll { namespace details {

#if __cplusplus >= 201402L

#define HLL_CONSTEXPR_OR_INLINE constexpr

#else

#define HLL_CONSTEXPR_OR_INLINE inline

#endif // __cplusplus >= 201402L


} // namespace details
} // namespace hll

#endif // HLL_DETAILS_HXX
