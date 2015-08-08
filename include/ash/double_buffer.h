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
// Implementation from:
// http://stackoverflow.com/questions/23666069/single-producer-single-consumer-data-structure-with-double-buffer-in-c/23713793#23713793

#pragma once

#include <atomic>
#include <cassert>
#include <cstdint>
#include <memory>
#include <utility>

namespace ash {

template<typename T> class double_buffer;

template<typename T>
class double_buffer_wr_guard
{
public:
    using parent_type = double_buffer<T>;
    friend parent_type;

protected:
    double_buffer_wr_guard(parent_type& p) :
        data_{p.begin_write()},
        parent_{p}
    { }

    ~double_buffer_wr_guard()
    {
        parent_.end_write();
    }

    T& get() const { return data_; }

private:
    T& data_;
    parent_type& parent_;
};

template<typename T>
class double_buffer_rd_guard
{
public:
    using parent_type = double_buffer<T>;
    friend parent_type;

protected:
    explicit double_buffer_rd_guard(parent_type& p) :
        data_{p.begin_read()},
        parent_{p}
    { }

    ~double_buffer_rd_guard()
    {
        parent_.end_read();
    }


    const T* get() const { return data_; }

private:
    const T* const data_;
    parent_type& parent_;
};

template<typename T>
class double_buffer_rd_lock
{
public:
    using parent_type = double_buffer<T>;
    using self_type = double_buffer_rd_lock<T>;

public:
    explicit double_buffer_rd_lock(parent_type& p) :
        data_{p.begin_read()},
        parent_{std::addressof(p)}
    { }

    double_buffer_rd_lock(self_type&& src) :
        data_{src.data_},
        parent_{src.parent_}
    {
        src.data_ = nullptr;
        src.parent_ = nullptr;
    }

    double_buffer_rd_lock(const self_type&) = delete;
    self_type& operator=(const self_type&) = delete;

    ~double_buffer_rd_lock()
    {
        if (parent_)
            parent_->end_read();
    }

    explicit operator bool() const noexcept
    {
        return data_ != nullptr and parent_ != nullptr;
    }

    operator const T&() const
    {
        return get();
    }

    const T& get() const
    {
        assert(data_ != nullptr);
        return *data_;
    }

private:
    const T* data_{nullptr};
    double_buffer<T>* parent_{nullptr};
};

template<typename T>
class double_buffer
{
public:
    using read_lock = double_buffer_rd_lock<T>;
    friend read_lock;

protected:
    using read_guard    = double_buffer_rd_guard<T>;
    using write_guard   = double_buffer_wr_guard<T>;

    friend write_guard;
    friend read_guard;

public:
    double_buffer() = default;

    explicit double_buffer(const T& value) :
        buffers_{value, value}
    { }

    double_buffer(const T& value1, const T& value2) :
        buffers_{value1, value2}
    { }

    double_buffer(const double_buffer& src) :
        buffers_{src.buffers_}
    { }

    double_buffer(double_buffer&& src) :
        buffers_{src.buffers_}
    { }

    bool is_lock_free() const
    {
        return state_.is_lock_free();
    }

    void write(const T& src)
    {
        write_guard{*this}.get() = src;
    }

    void write(T&& src)
    {
        write_guard{*this}.get() = std::move(src);
    }

    template<typename... Args>
    void emplace(Args&&... args)
    {
        write_guard{*this}.get() = T(std::forward<Args>(args)...);
    }

    bool try_read(T& dest)
    {
        const read_guard rg{*this};
        const auto* src = rg.get();
        if (src) {
            dest = (*src);
        }
        return src != nullptr;
    }

    read_lock make_read_lock() noexcept
    {
        return read_lock{*this};
    }

protected:
    T& begin_write() noexcept
    {
        // Increment active users; once we do this, no one can swap the active
        // cell on us until we're done
        auto state = state_.fetch_add(0x2, std::memory_order_relaxed);
        return buffers_[state & 1];
    }

    void end_write() noexcept
    {
        // We want to swap the active cell, but only if we were the last ones
        // concurrently accessing the data (otherwise the consumer will do it
        // for us when *it's* done accessing the data)
        auto state = state_.load(std::memory_order_relaxed);
        std::uint32_t flag = (8 << (state & 1)) ^ (state & (8 << (state & 1)));
        state = state_.fetch_add(flag - 0x2, std::memory_order_release) +
            flag - 0x2;
        if ((state & 0x6) == 0) {
            // The consumer wasn't in the middle of a read, we should swap
            // (unless the consumer has since started a read or already swapped
            // or read a value and is about to swap).  If we swap, we also want
            // to clear the full flag on what will become the active cell,
            // otherwise the consumer could eventually read two values out of
            // order (it reads a new value, then swaps and reads the old value
            // while the producer is idle).
            state_.compare_exchange_strong(
                    state, (state ^ 0x1) & ~(0x10 >> (state & 1)),
                    std::memory_order_release);
        }
    }

    const T* begin_read() noexcept
    {
        read_state_ = state_.load(std::memory_order_relaxed);
        if ((read_state_ & (0x10 >> (read_state_ & 1))) == 0) {
            // Nothing to read here!
            return nullptr;
        }

        // At this point, there is guaranteed to be something to read, because
        // the full flag is never turned off by the producer thread once it's
        // on; the only thing that could happen is that the active cell
        // changes, but that can only happen after the producer wrote a value
        // into it, in which case there's still a value to read, just in a
        // different cell.

        read_state_ = state_.fetch_add(0x2, std::memory_order_acquire) + 0x2;

        // Now that we've incremented the user count, nobody can swap until we
        // decrement it
        return std::addressof(buffers_[(read_state_ & 1) ^ 1]);
    }

    void end_read() noexcept
    {
        if ((read_state_ & (0x10 >> (read_state_ & 1))) == 0) {
            // There was nothing to read; shame to repeat this check, but if
            // these functions are inlined it might not matter. Otherwise the
            // API could be changed.  Or just don't call this method if
            // begin_read() returns nullptr -- then you could also get rid
            // of read_state_.
            return;
        }

        // Alright, at this point the active cell cannot change on us, but the
        // active cell's flag could change and the user count could change. We
        // want to release our user count and remove the flag on the value we
        // read.

        auto state = state_.load(std::memory_order_relaxed);
        std::uint32_t sub = (0x10 >> (state & 1)) | 0x2;
        state = state_.fetch_sub(sub, std::memory_order_relaxed) - sub;
        if ((state & 0x6) == 0 && (state & (0x8 << (state & 1))) == 1) {
            // Oi, we were the last ones accessing the data when we released
            // our cell.  That means we should swap, but only if the producer
            // isn't in the middle of producing something, and hasn't already
            // swapped, and hasn't already set the flag we just reset (which
            // would mean they swapped an even number of times).  Note that we
            // don't bother swapping if there's nothing to read in the other
            // cell.
            state_.compare_exchange_strong(
                    state, state ^ 0x1, std::memory_order_relaxed);
        }
    }

private:
    T buffers_[2];

    // The bottom (lowest) bit will be the active cell (the one for writing).
    // The active cell can only be switched if there's at most one concurrent
    // user. The next two bits of state will be the number of concurrent users.
    // The fourth bit indicates if there's a value available for reading
    // in buffers_[0], and the fifth bit has the same meaning but for
    // buffers_[1].
    std::atomic<std::uint32_t> state_{0};

    std::uint32_t read_state_{0};
};

} // namespace ash
