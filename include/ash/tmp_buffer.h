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

#include <stdexcept>
#include <cstdint>
#include <memory>
#include <utility>

namespace ash {

template<class T>
class unique_tmp_buffer {
public:
    using reference = T&;
    using const_reference = const T&;
    using pointer = T*;
    using iterator = pointer;
    using const_iterator = const T*;
    using size_type = std::ptrdiff_t;

public:
    unique_tmp_buffer(size_type n) :
        data_{std::get_temporary_buffer<T>(n)}
    { }

    unique_tmp_buffer(unique_tmp_buffer&& src) :
        data_{src.data_}
    {
        src.data_ = { nullptr, size_type{0} };
    }

    ~unique_tmp_buffer()
    {
        if (data_.first) {
            std::return_temporary_buffer(data_.first);
        }
    }

    unique_tmp_buffer(const unique_tmp_buffer&) = delete;

    void reset(size_type n = 0)
    {
        if (data_.first) {
            std::return_temporary_buffer(data_.first);
        }
        data_ = std::get_temporary_buffer<T>(n);
    }

public:
    reference at(size_type i)
    {
        if (i >= size()) {
            throw std::out_of_range("ash::unique_tmp_buffer::at");
        }
        return data_.first[i];
    }

    const_reference at(size_type i) const
    {
        if (i >= size()) {
            throw std::out_of_range("ash::unique_tmp_buffer::at");
        }
        return data_.first[i];
    }

    reference operator[](size_type i)               { return data_.first[i]; }
    const_reference operator[](size_type i) const   { return data_.first[i]; }

    pointer data() const    { return data_.first; }
    pointer get() const     { return data(); }
    size_type size() const  { return data_.second; }

    iterator begin() const          { return data_.first; }
    iterator end() const            { return begin() + size(); }
    const_iterator cbegin() const   { return data_.first; }
    const_iterator cend() const     { return cbegin() + size(); }

    explicit operator bool() const
    {
        return data() != nullptr;
    }

private:
    std::pair<pointer, size_type> data_;
};

} // namespace ash
