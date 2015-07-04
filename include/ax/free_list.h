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
#include <cassert>
#include <cstdint>

namespace ax {

template<typename T> class free_list;

template<typename T>
struct free_list_node {
    std::atomic<T*> _ax_fl_next{nullptr};
};

template<typename T>
class free_list
{
public:
private:
    static const constexpr std::uint32_t REF_MASK = 0x7FFFFFFF;
    static const constexpr std::uint32_t ON_LIST = 0x80000000;

    struct head_type {
        T* value = nullptr;
        std::uintptr_t tag = 0;
    };

public:
    inline
    void add(T* node)
    {
        assert(node);
        auto head = head_.load(std::memory_order_relaxed);
        head_type next_head = { node, 0 };
        do {
            next_head.tag = head.tag + 1;
            node->_ax_fl_next.store(head.value, std::memory_order_relaxed);
        } while (not swap_heads(head, next_head, std::memory_order_relaxed));
    }

    inline
    T* try_get()
    {
        auto head = head_.load(std::memory_order_acquire);
        head_type next_head;
        while (head.value != nullptr) {
            next_head.value = head.value->_ax_fl_next.load(std::memory_order_relaxed);
            next_head.tag = head.tag + 1;
            if (swap_heads(head, next_head, std::memory_order_acquire))
                break;
        }
        return head.value;
    }

private:
    inline
    bool swap_heads(head_type& head, head_type& next_head, std::memory_order mo)
    {
        return head_.compare_exchange_strong(
            head, next_head,
            std::memory_order_release, mo);
    }

private:
    std::atomic<head_type> head_;
};

} // namespace ax
