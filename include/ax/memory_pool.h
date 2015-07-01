// Lock free memory pool
//

#pragma once

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

    static inline
    storage_type& storage()
    {
        static memory_pool<T> instance;
        return instance.storage_;
    }

    ~memory_pool()
    {
        while (auto* obj = storage_.try_get()) {
            ::operator delete(obj);
        }
    }

private:
    storage_type storage_;
};

} // namespace details

template<typename T>
class memory_pooled : public ax::free_list_node<memory_pooled<T>>
{
public:
    using pool_type = details::memory_pool<T>;

public:
    static
    void* operator new(std::size_t sz)
    {
        if (auto* old = pool_type::storage().try_get()) {
            std::cout << "Reusing: " << old << std::endl;
            return old;
        }
        void* x = ::operator new(sz);
        std::cout << "Allocating: " << x << std::endl;
        return x;
    }

    static
    void operator delete(void* ptr)
    {
        std::cout << "Deleting: " << ptr << std::endl;
        pool_type::storage().add(reinterpret_cast<memory_pooled<T>*>(ptr));
    }
};

} // namespace ax
