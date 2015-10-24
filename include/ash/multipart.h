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

#include <memory>
#include <utility>

namespace ash {

template<class...> class multipart;

template<class... Ts>
class multipart : multipart<Ts>... {
public:
    multipart() = default;

    multipart(std::nullptr_t) :
        multipart<Ts>{nullptr}...
    { }

    template<class T, typename... Args>
    void make(Args&&... args)
    {
        multipart<T>::make(std::forward<Args>(args)...);
    }

    template<class T>
    T& get() { return multipart<T>::get(); }

    template<class T>
    const T& get() const { return multipart<T>::get(); }

    template<class T>
    T* get_ptr() const { return multipart<T>::get_ptr(); }
};

template<class T>
class multipart<T> {
public:
    multipart() :
        data_{std::make_unique<T>()}
    { }

    multipart(std::nullptr_t) :
        data_{nullptr}
    { }

    template<typename... Args>
    void make(Args&&... args)
    {
        data_ = std::make_unique<T>(std::forward<Args>(args)...);
    }


    T& get()                { return *data_; }
    const T& get() const    { return *data_; }
    T* get_ptr() const      { return data_.get(); }

private:
    std::unique_ptr<T> data_{nullptr};
};

} // namespace ash
