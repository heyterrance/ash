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

#include <Functional>
#include <utility>

namespace ash {

template<typename> class function_ptr;

template<typename Ret, typename... Args>
class function_ptr<Ret(Args...)>
{
public:
    using pointer = Ret (*)(Args...);

public:
    function_ptr() = default;

    template<typename Func>
    explicit function_ptr(Func&& f) :
        ptr_{f}
    { }

    function_ptr(const function_ptr&) = default;

    function_ptr(function_ptr&& src) :
        ptr_{src.ptr_}
    {
        src.ptr_ = nullptr;
    }

    function_ptr& operator=(pointer f)
    {
        ptr_ = f;
        return *this;
    }

    function_ptr& operator=(const function_ptr& src) = default;

    function_ptr& operator=(function_ptr&& src)
    {
        ptr_ = src.ptr_;
        src.ptr_ = nullptr;
        return *this;
    }

    pointer get() const
    {
        return ptr_;
    }

    explicit operator bool() const noexcept
    {
        return ptr_ != nullptr;
    }

    void swap(function_ptr& other) noexcept
    {
        auto* tmp = other.ptr_;
        other.ptr_ = ptr_;
        ptr_ = tmp;
    }

    void assign(pointer f) noexcept
    {
        ptr_ = f;
    }

    Ret operator()(Args&&... args) const throw(std::bad_function_call)
    {
        if (not *this) {
            throw std::bad_function_call{};
        }
        return ptr_(std::forward<Args>(args)...);
    }

private:
    pointer ptr_ = nullptr;
};

} // namespace ash

namespace std {

template<typename R, typename... Args>
struct hash<ash::function_ptr<R(Args...)>> :
    private hash<typename ash::function_ptr<R(Args...)>::pointer>
{
    using argument_type = ash::function_ptr<R(Args...)>;
    using base_type = hash<typename argument_type::pointer>;
    using result_type = typename base_type::result_type;

    result_type operator()(const argument_type& src) const
    {
        return static_cast<const base_type&>(*this)(src.get());
    }
};

template<typename R, typename... Args>
void swap(
        ash::function_ptr<R(Args...)>& lhs,
        ash::function_ptr<R(Args...)>& rhs)
{
    lhs.swap(rhs);
}

} // namespace std
