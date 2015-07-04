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

#include <atomic>
#include <cstdint>
#include <memory>

namespace ax {

template<
    typename T, std::size_t kCapacity,
    std::size_t kCacheLineSize = 64>
class data_region
{
public:
    using size_type = std::size_t;

    using value_type = T;

    using storage_type =
        std::aligned_storage_t<sizeof(value_type), kCacheLineSize>;

public:
    data_region() :
        data_(new storage_type[kCapacity])
    { }

    ~data_region()
    {
        for (size_type i = 0; i != size(); ++i) {
            get(i).~value_type();
        }
    }

    size_type reserve_next()
    {
        return size_++;
    }

    template<typename... Args>
    T& place(size_type pos, Args&&... args)
    {
        new (&data_[pos]) value_type(std::forward<Args>(args)...);
        return get(pos);
    }

    template<typename... Args>
    size_type place_next(Args&&... args)
    {
        const size_type pos = reserve_next();
        place(pos, std::forward<Args>(args)...);
        return pos;
    }

    T& get(size_type pos) const
    {
        return *reinterpret_cast<value_type*>(&data_[pos]);
    }

    size_type size() const  { return size_ - 1; }
    size_type empty() const { return size_ == 0; }

private:
    const std::unique_ptr<storage_type[]> data_;
    std::atomic_size_t size_{0};
};

} // namespace ax
