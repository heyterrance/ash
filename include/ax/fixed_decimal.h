// Fixed precision number
//

#pragma once

#include <limits>
#include <ostream>
#include <string>

#include "c_utils.h"
#include "compare_base.h"
#include "ax_math.h"

namespace details {

template<typename T, T Base, unsigned char E, unsigned char F>
using scaling_factor = ax::c_pow<T, Base, F - std::min(F, E)>;

static_assert(scaling_factor<int, 10, 3, 4>::value == 10, "Wrong scaling factor.");
static_assert(scaling_factor<int, 10, 2, 5>::value == 1000, "Wrong scaling factor.");
static_assert(scaling_factor<int, 10, 5, 5>::value == 1, "Wrong scaling factor.");
static_assert(scaling_factor<int, 10, 4, 3>::value == 1, "Wrong scaling factor.");

} // namespace details

namespace ax {

template<unsigned char E>
class fixed_decimal : compareable<fixed_decimal<E>>
{
private:
    using self_type = fixed_decimal<E>;

public:
    template<unsigned char X, unsigned char Y>
    using scaling_type = details::scaling_factor<long long, 10, X, Y>;
    template<unsigned char F> using self_mul = scaling_type<E, F>;
    template<unsigned char F> using other_mul = scaling_type<F, E>;

    static const constexpr unsigned char Exponent = E;
    static const constexpr auto Multiplier = c_pow<long long, 10, E>::value;

public:
    fixed_decimal() = default;

    template <
        typename T,
        std::enable_if_t<std::is_arithmetic<T>::value, int> = 0>
    constexpr
    fixed_decimal(T value) :
        value_(value * Multiplier)
    { }

    template <
        typename T,
        std::enable_if_t<std::is_arithmetic<T>::value, int> = 0>
    constexpr explicit
    fixed_decimal(T numer, T denom) :
        value_((numer * Multiplier * Multiplier) / (denom * Multiplier))
    { }

    template <
        unsigned char F,
        long long FM = other_mul<F>::value,
        long long EM = self_mul<F>::value>
    constexpr explicit
    fixed_decimal(const fixed_decimal<F>& src) :
        value_((FM * src.as_llong()) / EM)
    { }

public:
    static inline constexpr
    long long multiplier()
    {
        return Multiplier;
    }

    static inline constexpr
    unsigned char exponent()
    {
        return Exponent;
    }

    static inline constexpr
    self_type from_llong(long long v)
    {
        self_type r;
        r.value_ = v;
        return r;
    }

    static inline constexpr
    self_type from_string(const std::string& src)
    {
        return from_string(src.c_str());
    }

    static inline constexpr
    self_type from_string(const char* src)
    {
        long long mant = 0;
        if (*src == '-')
            return -from_string(++src);

        while (*src != '\0' and *src != '.') {
            mant = (mant * 10) + (*src++ - '0');
        }
        if (*src++ == '\0')
            return from_llong(mant * Multiplier);
        long long frac = 0;
        long long mul = Multiplier;
        while (*src != '\0' and mul != 0) {
            mul /= 10;
            frac += (*(src++) - '0') * mul;
        }
        return from_llong(mant * Multiplier + frac);
    }

public:
    template<
        unsigned char F,
        long long FM = other_mul<F>::value,
        long long EM = self_mul<F>::value>
    constexpr
    bool operator==(const fixed_decimal<F>& src) const
    {
        return (EM * value_) == (src.as_llong() * FM);
    }

    template<
        typename T,
        std::enable_if_t<std::is_arithmetic<T>::value, int> = 0>
    constexpr
    bool operator==(T value) const
    {
        return value_ == (value * Multiplier);
    }

    template<
        unsigned char F,
        long long FM = other_mul<F>::value,
        long long EM = self_mul<F>::value>
    constexpr
    bool operator<(const fixed_decimal<F>& src) const
    {
        return (EM * value_) < (src.as_llong() * FM);
    }

    template<
        typename T,
        std::enable_if_t<std::is_arithmetic<T>::value, int> = 0>
    constexpr
    bool operator<(T value) const
    {
        return value_ < (value * Multiplier);
    }

    template<
        typename T,
        std::enable_if_t<std::is_arithmetic<T>::value, int> = 0>
    constexpr
    self_type& operator=(T value)
    {
        value_ = value * Multiplier;
        return *this;
    }

    constexpr
    self_type& operator-()
    {
        value_ = -value_;
        return *this;
    }

    constexpr
    self_type& operator++()
    {
        value_ += Multiplier;
        return *this;
    }

    constexpr
    self_type operator++(int)
    {
        const self_type ret(*this);
        ++(*this);
        return ret;
    }

    constexpr
    self_type& operator--()
    {
        value_ -= Multiplier;
        return *this;
    }

    constexpr
    self_type operator--(int)
    {
        const self_type ret(*this);
        --(*this);
        return ret;
    }

    constexpr
    long long as_llong() const
    {
        return value_;
    }

    constexpr
    void set_value(long long v)
    {
        value_ = v;
    }

    constexpr
    double as_double() const
    {
        return static_cast<double>(value_) / static_cast<double>(Multiplier);
    }

private:
    long long value_ = 0;
};

template<
    unsigned char E, typename T,
    std::enable_if_t<std::is_arithmetic<T>::value, int> = 0>
inline constexpr
fixed_decimal<E> operator/(T d, const fixed_decimal<E>& src)
{
    fixed_decimal<E> ret(d * src.multiplier());
    ret.set_value(ret.as_llong() /src.as_llong());
    return ret;
}

template<
    unsigned char E,
    unsigned char F,
    class ret_type = fixed_decimal<std::max(E, F)>,
    long long EM = details::scaling_factor<long long, 10, E, F>::value,
    long long FM = details::scaling_factor<long long, 10, F, E>::value>
inline constexpr
ret_type operator/(const fixed_decimal<E>& a, const fixed_decimal<F>& b)
{
    ret_type r0(a.as_llong() * EM);
    return r0 / (b.as_llong() * FM);
}

template<
    unsigned char E, typename T,
    std::enable_if_t<std::is_arithmetic<T>::value, int> = 0>
inline constexpr
fixed_decimal<E> operator/(const fixed_decimal<E>& src, T d)
{
    return fixed_decimal<E>::from_llong(src.as_llong() / d);
}

template<
    unsigned char E,
    unsigned char F,
    class ret_type = fixed_decimal<std::max(E, F)>,
    long long EM = details::scaling_factor<long long, 10, E, F>::value,
    long long FM = details::scaling_factor<long long, 10, F, E>::value>
inline constexpr
ret_type operator*(
        const fixed_decimal<E>& a,
        const fixed_decimal<F>& b)
{
    constexpr long long RM = ret_type::multiplier();
    return ret_type::from_llong(
            (a.as_llong() * EM * b.as_llong() * FM) / RM);
}


template<
    unsigned char E, typename T,
    std::enable_if_t<std::is_arithmetic<T>::value, int> = 0>
inline constexpr
fixed_decimal<E> operator*(T m, const fixed_decimal<E>& src)
{
    return fixed_decimal<E>::from_llong(src.as_llong() * m);
}

template<
    unsigned char E, typename T,
    std::enable_if_t<std::is_arithmetic<T>::value, int> = 0>
inline constexpr
fixed_decimal<E> operator*(const fixed_decimal<E>& src, T m)
{
    return m * src;
}

template<unsigned char E>
inline
std::ostream& operator<<(std::ostream& out, const fixed_decimal<E>& src)
{
    long long val = src.as_llong();
    long long divisor = src.multiplier();
    for (unsigned dig = 0; dig != src.exponent() + 1; ++dig) {
        out << (val / divisor);
        if (dig == 0)
            out << '.';
        val = std::abs(val % divisor);
        divisor /= 10;
    }
    return out;
}

} // namespace ax

namespace std {

template<unsigned char E>
struct numeric_limits<ax::fixed_decimal<E>> : numeric_limits<long long>
{
    using type = ax::fixed_decimal<E>;

    static const constexpr bool is_integer = false;
    static const constexpr bool is_specialized = true;

    static constexpr
    type min()
    {
        return type::from_llong(numeric_limits<long long>::min());
    }

    static constexpr
    type max()
    {
        return type::from_llong(numeric_limits<long long>::max());
    }
};

} // namespace std
