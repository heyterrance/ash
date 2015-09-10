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
#include <utility>

namespace ash {

namespace detail {

template<typename T, typename SeqT>
struct optimistic_page
{
    std::atomic<SeqT> sequence_{0};
    T value_;
};

template<typename T>
struct write_lock_bit :
    std::integral_constant<std::size_t, CHAR_BIT * sizeof(T) - 1>
{ };

} // namespace detail

template<typename T, std::size_t NPages=2>
class optimistic_buffer
{
private:
    using sequence_type = unsigned;
    using page_type = detail::optimistic_page<T, sequence_type>;
    using value_type = T;
    using size_type = std::size_t;

    static constexpr std::size_t WR_LOCK_BIT =
        detail::write_lock_bit<sequence_type>::value;
    static constexpr auto WR_LOCK_BYTE = sequence_type{1} << WR_LOCK_BIT;

public:
    optimistic_buffer() = default;

    optimistic_buffer(const optimistic_buffer&) = delete;
    optimistic_buffer& operator=(const optimistic_buffer&) = delete;

    template<typename Func>
    void write_with(Func&& f)
    {
        auto& dest = begin_write();
        f(dest);
        end_write();
    }

    template<typename... Args>
    void emplace(Args&&... args)
    {
        write_with([&](value_type& dest) { dest = value_type(std::forward<Args>(args)...); });
    }

    void write(const value_type& src)
    {
        write_with([&](value_type& dest){ dest = src; });
    }

    bool try_read(value_type& dest, unsigned retries=0) noexcept
    {
        const auto& page = pages_[rd_index_];
        const auto seq = page.sequence_.load(std::memory_order_acquire);
        const bool is_writing = has_wr_bit_set(seq);
        if (not is_writing)
            dest = page.value_;
        const bool valid_value =
            (not is_writing) and
            (seq == page.sequence_.load(std::memory_order_acquire));
        return
            (valid_value) ? true :
            (retries == 0) ? false :
            try_read(dest, retries - 1);
    }

private:
    value_type& begin_write() noexcept
    {
        auto& page = pages_[wr_index_];
        wr_sequence_ =
            page.sequence_.fetch_or(WR_LOCK_BYTE, std::memory_order_relaxed);
        return page.value_;
    }

    void end_write() noexcept
    {
        auto& page = pages_[wr_index_];
        ++wr_sequence_;
        const auto next_seq =
            has_wr_bit_set(wr_sequence_) ? sequence_type{0} :
            wr_sequence_;
        page.sequence_.store(next_seq, std::memory_order_relaxed);

        // Update read and write indices.
        rd_index_ = std::exchange(wr_index_, (wr_index_ + 1) % pages_.size());
    }

    template<typename U>
    static constexpr
    bool has_wr_bit_set(U&& u)
    {
        return (u & WR_LOCK_BYTE) != 0;
    }

private:
    std::array<page_type, NPages> pages_;
    size_type wr_index_{0}, rd_index_{0};
    sequence_type wr_sequence_{0};
};

} // namespace ash
