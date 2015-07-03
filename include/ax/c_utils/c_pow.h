#pragma once

#include <type_traits>

namespace details {

template<typename T, T B, signed char E>
struct pos_pow : std::integral_constant<T, B * pos_pow<T, B, E-1>::value> { };

template<typename T, T B>
struct pos_pow<T, B, 0> : std::integral_constant<T, 1> { };

template<typename T, T B, signed char E>
struct neg_pow : pos_pow<T, T(1) / B, -E> { };

} // namespace details

namespace ax {

template<typename T, T Base, signed char Exp>
struct c_pow :
    std::conditional<
        (Exp >= 0),
        details::pos_pow<T, Base, Exp>,
        details::neg_pow<T, Base, Exp>
    >::type
{ };

template<typename T, signed char Exp>
struct c_exp
{
    template<
        signed char E = Exp,
        typename std::enable_if_t<(E > 0), int> = 0>
    inline constexpr
    T operator()(const T& base) const
    {
        return
            c_exp<T, Exp / 2>()(base) *
            c_exp<T, (Exp + 1) / 2>()(base);
    }

    template<
        signed char E = Exp,
        typename std::enable_if_t<(E < 0), int> = 0>
    inline constexpr
    T operator()(const T& base) const
    {
        return c_exp<T, -Exp>()(T(1) / base);
    }
};

template<typename T>
struct c_exp<T, 0>
{
    inline constexpr
    T operator()(const T&) const
    {
        return T(1);
    }
};

template<typename T>
struct c_exp<T, 1>
{
    inline constexpr
    const T& operator()(const T& base) const
    {
        return base;
    }
};

} // namespace ax
