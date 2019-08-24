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
#include <stdexcept> // std::invalid_argument
#include "hash.hxx"
#include "helpers.hxx" // hll::helpers::max

namespace hll
{

/**
 * @brief HyperLogLog C++11 generic implementation
 * @tparam T the type of values
 * @tparam Allocator allocator
 */
template <typename T, std::size_t k>
class hyper_log_log
{
public:
    /// type of registers of the data structure
    using register_type = typename Allocator::value_type;
    /// type of size values
    using size_type = size_t;
    using value_type = T;

private:
    using container_type = std::array<register_type, registers_count>;
    size_type registers_count;
    uint8_t k_alternative;
    double alpha_m_squared;

    static uint32_t count_bits(hash_result value);
    container_type registers{};
public:
    /**
     * Default and parametrized constructor
     * @param k determines registers count
     */
    explicit hyper_log_log(uint8_t k = 15);

    ~hyper_log_log() = default;

    /*!
     * Get unique numbers count
     * \return - the count
     */
    size_type count() const;

    /*!
     * Add an element
     * \param value - the element
     */
    void add(const value_type &value);

    /*!
     * Get relative error of the data structure
     * \return - the error
     */
    double get_relative_error() const
    {
        return 1.04 / std::sqrt(registers_count);
    }

    /*!
     * Clear the data structure
     */
    void clear()
    {
        registers.fill({});
    }
};

template <typename T, std::size_t k>
uint32_t hyper_log_log<T, k>::count_bits(hash_result value) noexcept
{
    if ((value & 1) == 1)
        return 0;

    uint32_t c = 1;
    if ((value & 0b1111'1111'1111'1111) == 0)
    {
        value >>= 16;
        c += 16;
    }
    if ((value & 0b1111'1111) == 0)
    {
        value >>= 8;
        c += 8;
    }
    if ((value & 0b1111) == 0)
    {
        value >>= 4;
        c += 4;
    }
    if ((value & 0b0011) == 0)
    {
        value >>= 2;
        c += 2;
    }
    c -= value & 0b0001;

    return c;
}


template <typename T, std::size_t k>
auto hyper_log_log<T, k>::count() const
        -> typename hyper_log_log<T, k>::size_type
{
    constexpr double two_32_power = 0x100000000;
    double count = 0;

    for (const auto& element : registers)
        count += 1.0 / (1u << element);

    // Оценка количества элементов
    double estimation = alpha_m_squared / count;

    // корректировка результатов в зависимости от размеров оценки
    if (estimation <= 2.5 * registers_count)
    {
        auto zero_registers_count = std::count(registers.begin(), registers.end(), 0);

        if (zero_registers_count > 0)
            // если хотя бы один регистр "пустой", то используем linear counting
            estimation = registers_count * std::log(static_cast<double>(registers_count) / zero_registers_count);
    }
    else if (estimation > (two_32_power / 30.0))
    { // если оценка получилась довольно большой
        estimation = -two_32_power * std::log(1.0 - (estimation / two_32_power));
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
    registers[index] = static_cast<register_type>(std::max(static_cast<uint32_t>(registers[index]), rank));
}

} // namespace hll
#endif //HYPER_LOG_LOG_HXX
