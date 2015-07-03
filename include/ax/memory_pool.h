// Lock free memory pool
//

#pragma once

#include <atomic>
#include <new>
#include <type_traits>

#include "free_list.h"

namespace ax {

template<typename T> class memory_pooled;

namespace details {

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
    using storage_type = ax::free_list<pooled_type>;

    ~memory_pool()
    {
        while (auto* obj = to_trash_.try_get()) {
            ::operator delete[](obj);
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
        auto* objs = reinterpret_cast<pooled_type*>(chunk);
        for (unsigned i = 1; i < chunk_sz; ++i) {
            self.storage_.add(&objs[i]);
        }
        std::cout << "Chunk size: " << chunk_sz << std::endl;
        return reinterpret_cast<void*>(&objs[0]);
    }

    static inline
    void destroy(void* ptr)
    {
        std::cout << "Destroying: " << ptr << std::endl;
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
    storage_type storage_;
    std::atomic_size_t chunk_size_{1};
    storage_type to_trash_;
};

} // namespace details

template<typename T>
class memory_pooled : public ax::free_list_node<memory_pooled<T>>
{
private:
    using pool_type = details::memory_pool<T>;

public:
    static inline
    void fill_pool(std::size_t n)
    {
        if (n == 0)
            return;
        auto* x = new T;
        fill_pool(n - 1);
        delete x;
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

} // namespace ax
