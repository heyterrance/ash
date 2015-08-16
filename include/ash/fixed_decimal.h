/*
 * Copyright 2015 Howard, Terrance <heyterrance@gmail.com>
 * Author: Howard, Terrance <heyterrance@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <limits>
#include <ostream>
#include <string>

#include "c_utils.h"
#include "compare_base.h"
#include "ash_math.h"

namespace ash {

namespace details {

template<typename T, T Base, unsigned char E, unsigned char F>
using scaling_factor = ash::c_pow<T, Base, F - std::min(F, E)>;

static_assert(scaling_factor<int, 10, 3, 4>::value == 10, "Wrong scaling factor.");
static_assert(scaling_factor<int, 10, 2, 5>::value == 1000, "Wrong scaling factor.");
static_assert(scaling_factor<int, 10, 5, 5>::value == 1, "Wrong scaling factor.");
static_assert(scaling_factor<int, 10, 4, 3>::value == 1, "Wrong scaling factor.");

} // namespace details

template<unsigned char E, typename IntegerT = long long>
class fixed_decimal : compareable<fixed_decimal<E, IntegerT>>
{
private:
    using self_type = fixed_decimal<E, IntegerT>;

    template<unsigned char F>
    using other_type = fixed_decimal<F, IntegerT>;

public:
    using value_type = IntegerT;

    template<unsigned char X, unsigned char Y>
    using scaling_type = details::scaling_factor<IntegerT, 10, X, Y>;
    template<unsigned char F> using self_mul = scaling_type<E, F>;
    template<unsigned char F> using other_mul = scaling_type<F, E>;

    static const constexpr unsigned char Exponent = E;
    static const constexpr auto Multiplier = c_pow<IntegerT, 10, E>::value;

public:
    fixed_decimal() = default;

    template <typename T>
    constexpr
    explicit fixed_decimal(T value) :
        value_{static_cast<IntegerT>(value * Multiplier)}
    { }

    template <
        unsigned char F, typename FInt,
        IntegerT FM = other_mul<F>::value,
        IntegerT EM = self_mul<F>::value>
    constexpr
    explicit fixed_decimal(const fixed_decimal<F, FInt>& src) :
        value_{(FM * src.raw_value()) / EM}
    { }

public:
    static constexpr
    value_type multiplier()
    {
        return Multiplier;
    }

    static constexpr
    unsigned char exponent()
    {
        return Exponent;
    }

    static constexpr
    self_type from_raw_value(value_type v, unsigned char vex = Exponent)
    {
        if (vex < Exponent)
            return from_raw_value(v * 10, vex + 1);
        else if (vex > Exponent)
            return from_raw_value(v / 10, vex - 1);
        self_type r;
        r.value_ = v;
        return r;
    }

    template<typename T>
    static constexpr
    self_type from_fraction(T numer, T denom)
    {
        return from_raw_value(
                (numer * Multiplier * Multiplier) /
                (denom * Multiplier));
    }

    static constexpr
    self_type from_string(const std::string& src)
    {
        return from_string(src.c_str());
    }

    static constexpr
    self_type from_string(const char* src)
    {
        IntegerT mant{0};
        if (*src == '-')
            return -from_string(++src);

        while (*src != '\0' and *src != '.') {
            mant = (mant * 10) + (*src++ - '0');
        }
        if (*src++ == '\0')
            return from_raw_value(mant * Multiplier);
        IntegerT frac{0};
        IntegerT mul{Multiplier};
        while (*src != '\0' and mul != 0) {
            mul /= 10;
            frac += (*(src++) - '0') * mul;
        }
        return from_raw_value(mant * Multiplier + frac);
    }

public:
    template<
        unsigned char F, typename FInt,
        IntegerT FM = other_mul<F>::value,
        IntegerT EM = self_mul<F>::value>
    constexpr
    bool operator==(const fixed_decimal<F, FInt>& rhs) const
    {
        return (EM * value_) == (rhs.raw_value() * FM);
    }

    template<typename T>
    constexpr
    bool operator==(T value) const
    {
        return value_ == (value * Multiplier);
    }

    template<
        unsigned char F, typename FInt,
        IntegerT FM = other_mul<F>::value,
        IntegerT EM = self_mul<F>::value>
    constexpr
    bool operator<(const fixed_decimal<F, FInt>& rhs) const
    {
        return (EM * value_) < (rhs.raw_value() * FM);
    }

    template<typename T>
    constexpr
    bool operator<(T rhs) const
    {
        return value_ < (rhs * Multiplier);
    }

    template<typename T>
    constexpr
    self_type& operator=(T rhs)
    {
        value_ = rhs * Multiplier;
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
    value_type raw_value() const
    {
        return value_;
    }

    constexpr
    operator double() const
    {
        return as_double();
    }

    constexpr
    double as_double() const
    {
        return static_cast<double>(value_) / static_cast<double>(Multiplier);
    }

private:
    IntegerT value_ = 0;
};

template<unsigned char E, typename IntegerT, typename T>
constexpr
fixed_decimal<E, IntegerT> operator/(
        T lhs, const fixed_decimal<E, IntegerT>& rhs)
{
    using return_type = fixed_decimal<E, IntegerT>;
    return return_type::from_raw_value(
            (lhs * rhs.multiplier() * return_type::multiplier()) /
            rhs.raw_value());
}

template<
    unsigned char E,
    unsigned char F,
    typename IntegerT,
    class ret_type = fixed_decimal<std::max(E, F), IntegerT>,
    IntegerT EM = details::scaling_factor<IntegerT, 10, E, F>::value,
    IntegerT FM = details::scaling_factor<IntegerT, 10, F, E>::value>
constexpr
ret_type operator/(
        const fixed_decimal<E, IntegerT>& lhs,
        const fixed_decimal<F, IntegerT>& rhs)
{
    ret_type r0(lhs.raw_value() * EM);
    return r0 / (rhs.raw_value() * FM);
}

template<unsigned char E, typename IntegerT, typename T>
constexpr
fixed_decimal<E, IntegerT> operator/(
        const fixed_decimal<E, IntegerT>& lhs,
        T rhs)
{
    return fixed_decimal<E, IntegerT>::from_raw_value(lhs.raw_value() / rhs);
}

template<
    unsigned char E,
    unsigned char F,
    typename IntegerT,
    class ret_type = fixed_decimal<std::max(E, F), IntegerT>,
    IntegerT EM = details::scaling_factor<IntegerT, 10, E, F>::value,
    IntegerT FM = details::scaling_factor<IntegerT, 10, F, E>::value>
constexpr
ret_type operator*(
        const fixed_decimal<E, IntegerT>& a,
        const fixed_decimal<F, IntegerT>& b)
{
    constexpr IntegerT RM{ret_type::multiplier()};
    return ret_type::from_raw_value(
            (a.raw_value() * EM * b.raw_value() * FM) / RM);
}


template<unsigned char E, typename IntegerT, typename T>
constexpr
fixed_decimal<E, IntegerT> operator*(
        T rhs, const fixed_decimal<E, IntegerT>& lhs)
{
    return fixed_decimal<E, IntegerT>::from_raw_value(lhs.raw_value() * rhs);
}

template<unsigned char E, typename IntegerT, typename T>
constexpr
fixed_decimal<E, IntegerT> operator*(
        const fixed_decimal<E, IntegerT>& lhs, T rhs)
{
    return rhs * lhs;
}

template<unsigned char E, typename IntegerT>
std::ostream& operator<<(std::ostream& out, const fixed_decimal<E, IntegerT>& src)
{
    auto val = src.raw_value();
    auto divisor = src.multiplier();
    for (unsigned dig = 0; dig != src.exponent() + 1; ++dig) {
        out << (val / divisor);
        if (dig == 0)
            out << '.';
        val = std::abs(val % divisor);
        divisor /= 10;
    }
    return out;
}

template<unsigned E>
using ufixed_decimal = fixed_decimal<E, unsigned long long>;

} // namespace ash

namespace std {

template<unsigned char E, typename IntegerT>
struct numeric_limits<ash::fixed_decimal<E, IntegerT>> : numeric_limits<IntegerT>
{
    using type = ash::fixed_decimal<E, IntegerT>;

    static const constexpr bool is_integer = false;
    static const constexpr bool is_specialized = true;

    static constexpr
    type min()
    {
        return type::from_raw_value(numeric_limits<IntegerT>::min());
    }

    static constexpr
    type max()
    {
        return type::from_raw_value(numeric_limits<IntegerT>::max());
    }
};

template<unsigned char E, typename IntegerT>
struct is_arithmetic<ash::fixed_decimal<E, IntegerT>> : is_arithmetic<IntegerT>
{ };

} // namespace std
