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
#include <cstdint>
#include <forward_list>
#include <string>
#include <type_traits>
#include <utility>

namespace ash {

template<typename T, std::size_t Capacity>
class stable_chunk
{
private:
    static_assert(Capacity != 0, "Capacity must be positive");

public:
    using buffer_type = typename std::aligned_storage_t<sizeof(T), alignof(T)>;
    using storage_type = std::array<buffer_type, Capacity>;
    using reference = T&;
    using size_type = typename storage_type::size_type;

public:
    ~stable_chunk()
    {
        for (size_type i = 0; i != index_; ++i) {
            reinterpret_cast<T*>(&storage_[i])->~T();
        }
    }

    template<typename... Args>
    reference create(Args&&... args) noexcept
    {
        auto* obj = new (&storage_[index_++]) T(std::forward<Args>(args)...);
        return *obj;
    }

    template<typename... Args>
    reference try_create(Args&&... args) throw(std::out_of_range)
    {
        const auto idx = index_++;
        if (idx >= Capacity) {
            index_ = Capacity;
            static const std::string errstr = "Cannot emplace to index=";
            throw std::out_of_range(errstr + std::to_string(idx));
        }
        auto* obj = new (&storage_[idx]) T(std::forward<Args>(args)...);
        return *obj;
    }

    size_type size() const noexcept
    {
        return index_;
    }

    bool full() const noexcept
    {
        return index_ == Capacity;
    }

    static constexpr
    size_type capacity()
    {
        return Capacity;
    }

private:
    size_type index_{0};
    storage_type storage_;
};

template<typename T, std::size_t ChunkCapacity>
class stable_storage
{
public:
    using chunk_type = stable_chunk<T, ChunkCapacity>;
    using reference = typename chunk_type::reference;
    using size_type = typename chunk_type::size_type;

public:
    template<typename... Args>
    reference create(Args&&... args) noexcept
    {
        auto& chunk = grab_chunk();
        return chunk.create(std::forward<Args>(args)...);
    }

    size_type chunk_count() const noexcept
    {
        return num_chunks_;
    }

    static constexpr
    size_type chunk_capacity()
    {
        return chunk_type::capacity();
    }

private:
    chunk_type& grab_chunk() noexcept
    {
        auto& chunk = chunks_.front();
        if (not chunk.full())
            return chunk;
        chunks_.emplace_front();
        ++num_chunks_;
        return grab_chunk();
    }

private:
    std::forward_list<chunk_type> chunks_{1};
    size_type num_chunks_{1};
};

} // namespace ash
