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

#include <utility>

namespace ash {

template<typename T, T Value>
class keep_val {
public:
    using const_reference = const T&;

public:
    template<typename... Args>
    keep_val(Args&&... args) :
        value_(std::forward<Args>(args)...)
    { }

    operator const T&() const   { return value_; }
    const_reference get() const { return value_; }

    template<typename U>
    void store(const U& src)
    {
        value_ = src;
    }

    template<typename U>
    void store(U&& src)
    {
        value_ = std::move(src);
    }

    keep_val& operator--()
    {
        if (value_ != Value)
            --value_;
        return *this;
    }

    keep_val& operator++()
    {
        if (value_ != Value)
            ++value_;
        return *this;
    }

    T operator--(int)
    {
        T res(value_);
        --(*this);
        return res;
    }

    T operator++(int)
    {
        T res(value_);
        ++(*this);
        return res;
    }

    keep_val& operator=(const T& src)
    {
        if (value_ != Value) {
            value_ = src;
        }
        return *this;
    }

    keep_val& operator=(T&& src)
    {
        if (value_ != Value) {
            value_ = std::move(src);
        }
        return *this;
    }

private:
    T value_;
};

template<bool Value>
struct keep_bool : keep_val<bool, Value> {
    keep_bool() :
        keep_val<bool, Value>(!Value)
    { }

    keep_bool& operator=(bool src)
    {
        using base_type = keep_val<bool, Value>;
        static_cast<base_type&>(*this) = src;
        return *this;
    }
};

using keep_true = keep_bool<true>;
using keep_false = keep_bool<false>;

template<char V>            using keep_char = keep_val<char, V>;
template<unsigned char V>   using keep_uchar = keep_val<unsigned char, V>;
template<short V>           using keep_short = keep_val<short, V>;
template<unsigned short V>  using keep_ushort = keep_val<unsigned short, V>;
template<int V>             using keep_int = keep_val<int, V>;
template<unsigned V>        using keep_uint = keep_val<unsigned , V>;
template<long V>            using keep_long = keep_val<long, V>;
template<unsigned long V>   using keep_ulong = keep_val<unsigned long, V>;
template<long long V>       using keep_llong = keep_val<long long, V>;
template<unsigned long long V>
using keep_ullong = keep_val<unsigned long long, V>;

} // namespace ash
