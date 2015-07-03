// Fixed length string
//

#pragma once

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <ostream>

#include "compare_base.h"

namespace ax {

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

    constexpr fixed_string(const char* s)
    {
        size_type idx = 0;
        while (idx != Capacity and *s != '\0') {
            data_[idx++] = *(s++);
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
    bool operator==(const fixed_string<M, CharT>& rhs) const
    {
        if (length() != rhs.length()) return false;
        for (std::size_t i = 0; i != rhs.length(); ++i)
            if (data_[i] != rhs[i]) return false;
        return true;
    }

    template<std::size_t M>
    constexpr
    bool operator<(const fixed_string<M, CharT>& rhs) const
    {
        return std::lexicographical_compare(
                begin(), end(), rhs.begin(), rhs.end());
    }

    friend std::ostream& operator<<(std::ostream&, const fixed_string&);

private:
    CharT data_[Capacity] = { 0 };
};

template<std::size_t C, typename T>
inline
std::ostream& operator<<(std::ostream& s, const fixed_string<C, T>& src)
{
    return s << src.data_;
}
} // namespace ax
