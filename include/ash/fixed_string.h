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

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <ostream>

#include "compare_base.h"

namespace ash {

template<std::size_t Capacity, typename CharT = char>
class fixed_string : compareable<fixed_string<Capacity, CharT>>
{
public:
    using value_type = CharT;
    using iterator = CharT*;
    using const_iterator = const CharT*;
    using size_type = std::size_t;

public:
    fixed_string() = default;

    template<typename Allocator>
    constexpr fixed_string(const std::basic_string<CharT, Allocator>& src) :
        fixed_string(src.c_str())
    { }

    constexpr fixed_string(const char* s, size_type count)
    {
        size_type idx = 0;
        while (idx != Capacity and *s != '\0' and count != 0) {
            data_[idx++] = *(s++);
            --count;
        }
    }

    constexpr fixed_string(const char* s)
    {
        size_type idx = 0;
        while (idx != Capacity and *s != '\0') {
            data_[idx++] = *(s++);
        }
    }

    constexpr fixed_string(size_type count, CharT ch)
    {
        for (size_type i = 0; i != std::min(count, Capacity); ++i) {
            data_[i] = ch;
        }
    }

    template<std::size_t N>
    constexpr fixed_string(const char (&s)[N])
    {
        static_assert(
                N >= Capacity,
                "Cannot instantiate with string larger than Capacity.");
        for (size_type i = 0; i != N; ++i)
            data_[i] = s[i];
    }

    //
    // Element access
    //

    constexpr
    CharT& operator[](size_type pos)
    {
        return data_[pos];
    }

    constexpr
    const CharT& operator[](size_type pos) const
    {
        return data_[pos];
    }

    constexpr       CharT& front()          { return data_[0]; }
    constexpr const CharT& front() const    { return data_[0]; }
    constexpr       CharT& back()           { return data_[length()]; }
    constexpr const CharT& back() const     { return data_[length()]; }
    constexpr const std::basic_string<CharT> str() const
    {
        return std::basic_string<CharT>(data_, length());
    }
    constexpr const CharT* c_str() const    { return data_; }
    constexpr const CharT* data() const     { return c_str(); }

    //
    // Iterators
    //

    constexpr iterator          begin()         { return data_; }
    constexpr const_iterator    begin() const   { return data_; }
    constexpr const_iterator    cbegin() const  { return data_; }
    constexpr iterator          end()           { return &back(); }
    constexpr const_iterator    end() const     { return &back(); }
    constexpr const_iterator    cend() const    { return &back(); }

    //
    // Capacity
    //

    constexpr
    size_type length() const
    {
        size_type idx = 0;
        while (idx != Capacity and data_[idx] != '\0') ++idx;
        return idx;
    }
    constexpr size_type size() const { return length(); }
    constexpr bool      empty() const { return data_[0] == '\0'; }
    static
    constexpr size_type capacity() { return Capacity; }
    static
    constexpr size_type max_size() { return Capacity; }

    //
    // Operations
    //

    void clear()
    {
        data_[0] = '\0';
    }

    fixed_string& insert(size_type index, size_type count, CharT ch)
    {
        for (size_type i = index; i < Capacity and count != 0; ++i, --count) {
            data_[i] = ch;
        }
        return *this;
    }

    fixed_string& insert(size_type index, const CharT* s)
    {
        assert(Capacity > index);
        std::strncpy(&data_[index], s, Capacity - index);
        return *this;
    }

    template<typename... Args>
    fixed_string& operator+=(Args&&... args)
    {
        return append(std::forward<Args>(args)...);
    }

    fixed_string& append(CharT ch)
    {
        return insert(length(), 1, ch);
    }

    fixed_string& append(size_type count, CharT ch)
    {
        return insert(length(), count, ch);
    }

    template<std::size_t Cap>
    fixed_string& append(const fixed_string<Cap, CharT>& s)
    {
        return append(s.c_str());
    }

    fixed_string& append(const CharT* s)
    {
        return insert(length(), s);
    }

    template<std::size_t M>
    constexpr
    bool operator==(const fixed_string<M, CharT>& rhs) const
    {
        const size_type len = length();
        if (len != rhs.length()) return false;
        for (std::size_t i = 0; i != len; ++i)
            if (data_[i] != rhs[i]) return false;
        return true;
    }

    constexpr
    bool operator==(const CharT (&rhs)[Capacity]) const
    {
        for (size_type i = 0; i != Capacity; ++i) {
            if (rhs[i] == '\0' and data_[i] == '\0')
                return true;
            if (rhs[i] != data_[i])
                return false;
        }
        return false;
    }

    template<typename Allocator>
    constexpr
    bool operator==(const std::basic_string<CharT, Allocator>& rhs) const
    {
        const size_type llen = length();
        if (llen != rhs.length()) return false;
        for (size_type i = 0; i != llen; ++i)
            if (data_[i] != rhs[i]) return false;
        return true;
    }

    template<size_type M, typename std::enable_if_t<(M < Capacity), int> = 0>
    constexpr
    bool operator==(const CharT (&rhs)[M]) const
    {
        size_type idx = 0;
        for (size_type i = 0; i != M; ++i) {
            if (rhs[i] == '\0' and data_[i] == '\0')
                return true;
            if (rhs[i] != data_[i])
                return false;
        }
        return data_[M] == '\0';
    }

    template<size_type M, typename std::enable_if_t<(M > Capacity), int> = 0>
    constexpr
    bool operator==(const CharT (&rhs)[M]) const
    {
        size_type idx = 0;
        for (size_type i = 0; i != Capacity; ++i) {
            if (rhs[i] == '\0' and data_[i] == '\0')
                return true;
            if (rhs[i] != data_[i])
                return false;
        }
        return rhs[Capacity] == '\0';
    }

    constexpr
    bool operator==(const CharT* rhs) const
    {
        const int res = std::strncmp(rhs, data_, Capacity);
        return (res == 0);
    }

    template<std::size_t M>
    constexpr
    bool operator<(const fixed_string<M, CharT>& rhs) const
    {
        return std::lexicographical_compare(
                begin(), end(), rhs.begin(), rhs.end());
    }

private:
    CharT data_[Capacity] = { 0 };
};

template<std::size_t C, typename T>
std::ostream& operator<<(std::ostream& s, const fixed_string<C, T>& src)
{
    return s << src.c_str();
}

} // namespace ash

namespace std {

template<std::size_t C, typename CharT>
struct hash<ash::fixed_string<C, CharT>>
{
    constexpr
    std::size_t operator()(const ash::fixed_string<C, CharT>& src) const
    {
        std::size_t h = 0;
        for (char c : src) {
            (h <<= 1) ^= static_cast<unsigned char>(c);
        }
        return h;
    }
};

} // namespace std
