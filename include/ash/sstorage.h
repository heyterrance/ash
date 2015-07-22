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

#include <array>
#include <atomic>
#include <cstdint>
#include <string>
#include <type_traits>
#include <utility>

namespace ash {

template<
    typename T, std::size_t Capacity,
    typename SizeT = std::size_t>
class stable_storage
{
public:
    using size_type = std::size_t;
    using reference = T&;
    using buffer_type = typename std::aligned_storage_t<sizeof(T), alignof(T)>;

public:
    ~stable_storage()
    {
        for (size_type i = 0; i != index_; ++i) {
            reinterpret_cast<T*>(&storage_[i])->~T();
        }
    }

    template<typename... Args>
    reference create(Args&&... args)
    {
        auto* obj = new (&storage_[index_++]) T(std::forward<Args>(args)...);
        return *obj;
    }

    template<typename... Args>
    reference try_create(Args&&... args)
    {
        const auto idx = index_++;
        if (idx >= Capacity) {
            index_ = idx;
            static const std::string errstr = "Cannot emplace to index=";
            throw std::out_of_range(errstr + std::to_string(idx));
        }
        auto* obj = new (&storage_[idx]) T(std::forward<Args>(args)...);
        return *obj;
    }

    size_type size() const
    {
        return index_;
    }

    static constexpr
    size_type capacity()
    {
        return Capacity;
    }

private:
    SizeT index_{0};
    std::array<buffer_type, Capacity> storage_;
};

template<typename T, std::size_t Capacity>
using sstorage = stable_storage<T, Capacity, std::size_t>;

template<typename T, std::size_t Capacity>
using atomic_sstorage = stable_storage<T, Capacity, std::atomic_size_t>;

} // namespace ash
