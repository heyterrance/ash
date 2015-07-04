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

namespace ash {

template<class Integer>
constexpr
unsigned num_digits(Integer n, unsigned char base = 10)
{
    unsigned c = 0;
    while (n != 0) {
        n /= base;
        ++c;
    }
    return c;
}

template<class Integer>
constexpr
unsigned first_digit(Integer n)
{
    if (n < 0)
        return first_digit(-n);
    while (n > 9) {
        n /= 10;
    }
    return n;
}

static_assert(num_digits(100, 10) == 3, "num_digits(100, 10) != 3");
static_assert(first_digit(54) == 5, "first_digit(54) != 5");
static_assert(first_digit(-80) == 8, "first_digit(-80) != 8");
}
