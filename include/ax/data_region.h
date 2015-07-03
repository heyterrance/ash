// Finite memory region
//

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
