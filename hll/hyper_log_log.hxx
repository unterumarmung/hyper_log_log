/**
 * @file hll/hyper_log_log.hxx
 * @brief HyperLogLog C++11 generic implementation
 * @author Daniil Dudkin (unterumarmung)
 */
#ifndef HYPER_LOG_LOG_HXX
#define HYPER_LOG_LOG_HXX

#include <algorithm> // std::count
#include <vector>
#include <cmath> // std::log
#include <stdexcept> // std::invalid_argument
#include <functional> // std::function
#include "murmur_hash.hxx"
#include "hash.hxx"

namespace hll
{

/**
 * @brief HyperLogLog C++11 generic implementation
 * @tparam T the type of values
 * @tparam Allocator allocator
 */
template <typename T, typename Allocator = std::allocator<int8_t>>
class hyper_log_log
{
public:
    /// type of registers of the data structure
    using register_type = typename Allocator::value_type;
    /// type of size values
    using size_type = size_t;
    using value_type = T;

private:
    std::vector<register_type, Allocator> registers;
    size_type registers_count;
    uint8_t k_alternative;
    double alpha_m_squared;

    static uint32_t count_bits(hash_result value);
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
        return 1.04 / sqrt(registers_count);
    }

    /*!
     * Clear the data structure
     */
    void clear()
    {
        registers.clear();
        registers.resize(registers_count);
    }
};

template<typename T, typename Allocator>
uint32_t hyper_log_log<T, Allocator>::count_bits(hash_result value)
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


template<typename T, typename Allocator>
hyper_log_log<T, Allocator>::hyper_log_log(uint8_t k)
{
    if (k < 4 || k >= 31)
        throw std::invalid_argument("k must be between [4 and 31)");

    // alpha используется в дальшейшем только в (32 - k), так что вычисляем заранее
    k_alternative = static_cast<uint8_t>(32 - k);

    // быстрое возведение в степень двойки
    registers_count = 1u << k;
    double alpha_m;

    // выбираем значение константы alpha в зависимости от кол-ва регистров
    switch (registers_count)
    {
        case 16:
            alpha_m = 0.673;
            break;
        case 32:
            alpha_m = 0.697;
            break;
        case 64:
            alpha_m = 0.709;
            break;
        default:
            alpha_m = 0.7213 /
                      (1.0 + 1.079 / registers_count);
    }

    // alpha используется в дальшейшем только с умножением на квадрат количества регистров, так что вычисляем заранее
    alpha_m_squared = alpha_m * registers_count * registers_count;

    registers.resize(registers_count);

}

template<typename T, typename Allocator>
auto hyper_log_log<T, Allocator>::count() const
        -> typename hyper_log_log<T, Allocator>::size_type
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

template<typename T, typename Allocator>
void hyper_log_log<T, Allocator>::add(const value_type &value)
{
    const auto hash_value = hll::hash(value);
    const auto index = hash_value >> k_alternative;
    const auto bits_count = count_bits(hash_value);
    const auto rank = std::min(static_cast<uint32_t>(k_alternative), bits_count) + 1;
    registers[index] = static_cast<register_type>(std::max(static_cast<uint32_t>(registers[index]), rank));
}

} // namespace hll
#endif //HYPER_LOG_LOG_HXX
