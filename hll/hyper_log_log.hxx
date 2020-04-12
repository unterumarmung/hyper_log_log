/**
 * @file hll/hyper_log_log.hxx
 * @brief HyperLogLog C++11 generic implementation
 * @author Daniil Dudkin (unterumarmung)
 */
#ifndef HYPER_LOG_LOG_HXX
#define HYPER_LOG_LOG_HXX

#include <algorithm> // std::count
#include <array>
#include <cmath> // std::log
#include "hash.hxx"
#include "helpers.hxx" // hll::helpers::max, hll::helpers::array_fill
#include "details.hxx" // HLL_CONSTEXPR_OR_INLINE

namespace hll
{

/**
 * @brief HyperLogLog C++11 generic implementation
 * @tparam T the type of values
 * @tparam k number that controls number of registers as 2^k
 */
template <typename T, std::size_t k>
class hyper_log_log
{
public:
    static_assert(k >= 4 && k <= 30, "k must be in a range [4; 30]");
    /// type of registers of the data structure
    using register_type = int8_t;
    /// type of size values
    using size_type = size_t;
    using value_type = T;
    using this_type = hyper_log_log;
    static constexpr size_type registers_count = 1u << k;

private:
    static constexpr double get_alpha_m() noexcept
    {
        return registers_count == 16
               ? 0.673
               : registers_count == 32
                 ? 0.697
                 : registers_count == 64
                   ? 0.709
                   : 0.7213 /
                     (1.0 + 1.079 / registers_count);
    }

    static HLL_CONSTEXPR_OR_INLINE uint32_t count_bits(hash_result value) noexcept;

    static constexpr auto k_alternative = static_cast<uint8_t>(32 - k);
    static constexpr auto alpha_m_squared = get_alpha_m() * registers_count * registers_count;

    using container_type = std::array<register_type, registers_count>;
    container_type m_registers{};
public:
    /**
     * Get unique numbers count
     * @return - the count
     */
    size_type count() const;

    /**
     * Add an element
     * @param value - the element
     */
    void add(const value_type &value);

    /**
     * Get relative error of the data structure
     * @return - the error
     */
    double get_relative_error() const
    {
        return 1.04 / std::sqrt(registers_count);
    }

    /**
     * Clear the data structure
     */
    constexpr void clear() noexcept(noexcept(hll::helpers::array_fill(m_registers, {})))
    {
        hll::helpers::array_fill(m_registers, {});
    }

    /**
     * HyperLogLog's merge operation
     * @param rhs A HyperLogLog instance to merge with
     * @return this reference
     */
    constexpr this_type& merge(const this_type& rhs) noexcept(noexcept(helpers::max<register_type>({}, {})));
    /**
     * HyperLogLog's merge operator overload
     * @param rhs A HyperLogLog instance to merge with
     * @return this reference
     */
    constexpr this_type& operator+=(const this_type& rhs) noexcept(noexcept(this->merge(rhs)));
    /**
     * Merges two HyperLogLog instances into a new one
     * @param rhs second HyperLogLog instance
     * @return Merged instance
     */
    constexpr this_type operator+(const this_type& rhs) const noexcept(noexcept(this->merge(rhs)));
};

template <typename T, std::size_t k>
HLL_CONSTEXPR_OR_INLINE uint32_t hyper_log_log<T, k>::count_bits(hash_result value) noexcept
{
    if ((value & 1u) == 1)
        return 0;

    uint32_t c = 1;
    if ((value & 0b1111'1111'1111'1111u) == 0)
    {
        value >>= 16u;
        c += 16;
    }
    if ((value & 0b1111'1111u) == 0)
    {
        value >>= 8u;
        c += 8;
    }
    if ((value & 0b1111u) == 0)
    {
        value >>= 4u;
        c += 4;
    }
    if ((value & 0b0011u) == 0)
    {
        value >>= 2u;
        c += 2;
    }
    c -= value & 0b0001u;

    return c;
}


template <typename T, std::size_t k>
auto hyper_log_log<T, k>::count() const
        -> typename hyper_log_log<T, k>::size_type
{
    constexpr double TWO_32_POWER = 0x100000000;
    double count = 0;

    for (const auto& element : m_registers)
        count += 1.0 / (1u << element);

    // Оценка количества элементов
    auto estimation = alpha_m_squared / count;

    // корректировка результатов в зависимости от размеров оценки
    if (estimation <= 2.5 * registers_count)
    {
        const auto zero_registers_count = std::count(m_registers.begin(), m_registers.end(), 0);

        if (zero_registers_count > 0)
            // если хотя бы один регистр "пустой", то используем linear counting
            estimation = registers_count * std::log(static_cast<double>(registers_count) / zero_registers_count);
    }
    else if (estimation > (TWO_32_POWER / 30.0))
    { // если оценка получилась довольно большой
        estimation = -TWO_32_POWER * std::log(1.0 - (estimation / TWO_32_POWER));
    }

    return static_cast<size_type>(estimation);
}

template <typename T, std::size_t k>
void hyper_log_log<T, k>::add(const value_type &value)
{
    const auto hash_value = hll::hash(value);
    const auto index = hash_value >> k_alternative;
    const auto bits_count = count_bits(hash_value);
    const auto rank = std::min(static_cast<uint32_t>(k_alternative), bits_count) + 1;
    m_registers[index] = static_cast<register_type>(std::max(static_cast<uint32_t>(m_registers[index]), rank));
}

template<typename T, std::size_t k>
constexpr hyper_log_log<T, k>& hyper_log_log<T, k>::merge(const hyper_log_log::this_type &rhs)
                                                        noexcept(noexcept(helpers::max<register_type>({}, {})))
{
    for (auto i = 0u; i < registers_count; ++i)
    {
        m_registers[i] = hll::helpers::max(m_registers[i], rhs.m_registers[i]);
    }
    return *this;
}

template<typename T, std::size_t k>
constexpr hyper_log_log<T, k>&
hyper_log_log<T, k>::operator+=(const typename hyper_log_log::this_type &rhs)
        noexcept(noexcept(this->merge(rhs)))
{
    this->merge(rhs);
    return *this;
}

template<typename T, std::size_t k>
constexpr hyper_log_log<T, k>
hyper_log_log<T, k>::operator+(const typename hyper_log_log::this_type &rhs) const
        noexcept(noexcept(this->merge(rhs)))
{
    this_type res = *this;
    res += rhs;
    return res;
}

} // namespace hll
#endif //HYPER_LOG_LOG_HXX
