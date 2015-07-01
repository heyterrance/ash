#pragma once

#include <type_traits>
#include <utility>

namespace ax {

template<typename T, T a, T... args>
struct c_max
{
    static const constexpr T next_value = c_max<T, args...>::value;
    static const constexpr T value = (next_value < value) ? a : next_value;
};

template<typename T, T a>
struct c_max<T, a> : std::integral_constant<T, a> { };

template<typename T, T a, T... args>
struct c_min
{
    static const constexpr T next_value = c_min<T, args...>::value;
    static const constexpr T value = (next_value < value) ? next_value : a;
};

template<typename T, T a>
struct c_min<T, a> : std::integral_constant<T, a> { };

template<typename T, T a, T... args>
struct c_min_max
{
    static const constexpr std::pair<T, T> value = {
        c_min<T, args...>::value,
        c_max<T, args...>::value
    };
};

} // namespace ax
