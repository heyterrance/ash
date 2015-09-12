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

template<typename T, typename SeqT = unsigned>
struct optimistic_page
{
public:
    using value_type = T;
    using size_type = std::size_t;

public:
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
        write_with([&](value_type& dest){
                dest = value_type(std::forward<Args>(args)...);
            });
    }

    void write(const value_type& src)
    {
        write_with([&](value_type& dest){ dest = src; });
    }

    void wait_read(value_type& dest) const noexcept
    {
        while (not try_read_impl(dest)) continue;
    }

    bool try_read(value_type& dest, unsigned retries=0) const noexcept
    {
        do {
            if (try_read_impl(dest))
                return true;
        } while (0 != retries--);
        return false;
    }

    value_type& begin_write() noexcept
    {
        sequence_.fetch_add(1, std::memory_order_relaxed);
        return value_;
    }

    void end_write() noexcept
    {
        sequence_.fetch_add(1, std::memory_order_relaxed);
    }

    bool try_read_impl(value_type& dest) const noexcept
    {
        // Odd sequences numbers mean page is being written to.
        const auto seq = sequence_.load(std::memory_order_acquire);
        const bool is_writing = (seq % 2) != 0;
        if (not is_writing)
            dest = value_;
        return
            (not is_writing) and
            (seq == sequence_.load(std::memory_order_acquire));
    }

private:
    std::atomic<SeqT> sequence_{0};
    T value_;
};

template<typename T, std::size_t NPages = 2>
class optimistic_buffer
{
private:
    using sequence_type = unsigned;
    using page_type = optimistic_page<T, sequence_type>;
    using value_type = T;
    using size_type = std::size_t;

public:
    optimistic_buffer() = default;

    optimistic_buffer(const optimistic_buffer&) = delete;
    optimistic_buffer& operator=(const optimistic_buffer&) = delete;

    static constexpr
    size_type capacity()
    {
        return NPages;
    }

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
        write_with([&](value_type& dest){
                dest = value_type(std::forward<Args>(args)...);
            });
    }

    void write(const value_type& src)
    {
        write_with([&](value_type& dest){ dest = src; });
    }

    void wait_read(value_type& dest) const noexcept
    {
        while (not try_read_impl(dest)) continue;
    }

    bool try_read(value_type& dest, unsigned retries=0) const noexcept
    {
        do {
            if (try_read_impl(dest))
                return true;
        } while (0 != retries--);
        return false;
    }

private:
    value_type& begin_write() noexcept
    {
        auto& page = pages_[wr_index_];
        return page.begin_write();
    }

    void end_write() noexcept
    {
        auto& page = pages_[wr_index_];
        page.end_write();

        // Update read and write indices.
        const auto next_wr_idx = (wr_index_ + 1) % pages_.size();
        rd_index_ = std::exchange(wr_index_, next_wr_idx);
    }

    bool try_read_impl(value_type& dest) const noexcept
    {
        const auto& page = pages_[rd_index_];
        return page.try_read(dest);
    }

private:
    std::array<page_type, NPages> pages_;
    size_type wr_index_{0};
    volatile size_type rd_index_{0};
};

} // namespace ash
