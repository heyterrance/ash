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

#include <type_traits>

namespace ash {

namespace details {

template<typename T, T B, signed char E>
struct pos_pow : std::integral_constant<T, B * pos_pow<T, B, E-1>::value> { };

template<typename T, T B>
struct pos_pow<T, B, 0> : std::integral_constant<T, 1> { };

template<typename T, T B, signed char E>
struct neg_pow : pos_pow<T, T(1) / B, -E> { };

} // namespace details

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

} // namespace ash
