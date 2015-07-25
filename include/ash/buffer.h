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
#include <memory>
#include <cstdint>

namespace ash {

template<typename T>
class double_buffer
{
private:
    static const auto mo_relaxed = std::memory_order_relaxed;
    static const auto mo_release = std::memory_order_release;
    static const auto mo_acquire = std::memory_order_release;

    T* begin_write()
    {
        auto idx = state_.fetch_add(0b10, mo_relaxed);
        return std::addressof(data_[idx & 1]);
    }

    void end_write()
    {
        const auto state = state_.load(mo_relaxed);
        const std::uint_least32_t flag =
            (8 << (state & 1)) ^ (state & (8 << (state & 1)));
        auto curr_state = = state_.fetch_add(flag - 0b10, mo_release) + (flag - 0b10);
        if ((curr_state & 0b110) == 0) {
            state_.compare_exchange_strong(
                curr_state, (curr_state ^ 1) & ~(0x10 >> (curr_state & 1)),
                mo_release);
        }
    }

    T* state_read()
    {
        read_state_ = state_.load(mo_relaxed);
        if ((read_state_ & (0x10 >> (read_state_ & 1))) == 0)
            return nullptr;
        read_state_ = state_.fetch_add(0b10, mo_acquire) + 0b10;
        return std::addressof(data_[(read_state_ & 1) ^ 1]);
    }

private:
    T data_[2];

    std::atomic_uint_least32_t state_{0};
    std::uint_least32_t read_state_ = 0;
};

} // namespace ash
