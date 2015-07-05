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
#include <new>
#include <type_traits>

#include "free_list.h"

namespace ash {

template<typename T> class memory_pooled;

namespace details {

class del_node : public free_list_node<del_node>
{
public:
    del_node(void* m) :
        mem(m)
    { }

    ~del_node()
    {
        ::operator delete(mem);
    }

private:
    void* const mem;
};


template<typename T>
class memory_pool
{
private:
    using pooled_type = memory_pooled<T>;

    memory_pool() = default;
    memory_pool(const memory_pool&) = delete;
    memory_pool(memory_pool&&) = delete;
    memory_pool& operator=(const memory_pool&) = delete;

public:
    using storage_list = ash::free_list<pooled_type>;
    using trash_list = ash::free_list<del_node>;

    ~memory_pool()
    {
        while (auto* obj = to_trash_.try_get()) {
            delete obj;
        }
    }

    static inline
    void* alloc(std::size_t sz)
    {
        if (auto* old = instance().storage_.try_get())
            return old;
        return add_chunk(sz);
    }

    static inline
    void* add_chunk(std::size_t sz)
    {
        auto& self = instance();
        const std::size_t chunk_sz =
            self.chunk_size_.fetch_add((self.chunk_size_ + 1) / 2);
        void* chunk = ::operator new(sz * chunk_sz);
        self.to_trash_.add(new del_node(chunk));
        char* const cchunk = reinterpret_cast<char*>(chunk);
        for (unsigned i = 1; i < chunk_sz; ++i) {
            auto* obj = reinterpret_cast<pooled_type*>(cchunk + (i * sz));
            self.storage_.add(obj);
        }
        return chunk;
    }

    static inline
    void destroy(void* ptr)
    {
        instance().storage_.add(reinterpret_cast<pooled_type*>(ptr));
    }

private:
    static inline
    memory_pool& instance()
    {
        static memory_pool<T> obj;
        return obj;
    }

private:
    storage_list storage_;
    std::atomic_size_t chunk_size_{2};
    trash_list to_trash_;
};

} // namespace details

template<typename T>
class memory_pooled : public ash::free_list_node<memory_pooled<T>>
{
private:
    using pool_type = details::memory_pool<T>;

public:
    static inline
    void fill_pool(std::size_t n)
    {
        if (n == 0)
            return;
        using alloc_type = std::aligned_storage_t<sizeof(T), alignof(T)>;
        void* x = pool_type::alloc(sizeof(alloc_type));
        fill_pool(n - 1);
        pool_type::destroy(x);
    }

    static inline
    void* operator new(std::size_t sz)
    {
        return pool_type::alloc(sz);
    }

    static inline
    void operator delete(void* ptr)
    {
        return pool_type::destroy(ptr);
    }
};

} // namespace ash
