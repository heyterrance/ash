/*
 *            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
 *                    Version 2, December 2004
 *
 * Copyright (C) 2016 Howard, Terrance <heyterrance@gmail.com>
 * Author: Howard, Terrance <heyterrance@gmail.com>
 *
 * Everyone is permitted to copy and distribute verbatim or modified
 * copies of this license document, and changing it is allowed as long
 * as the name is changed.
 *
 *            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
 *   TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION
 *
 *  0. You just DO WHAT THE FUCK YOU WANT TO.
 */

/*
 * Copyright 2016 Howard, Terrance <heyterrance@gmail.com>
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

#include <cstddef>
#include <utility>
#include <tuple>

namespace ash {

template<typename T>
using dup_pair = std::pair<T, T>;

namespace detail {

template<std::size_t N, typename... Ts>
struct dup_tuple_impl;

template<typename T, typename... Ts>
struct dup_tuple_impl<0, T, Ts...>
{
    using type = typename std::tuple<Ts...>;
};

template<std::size_t N, typename T, typename... Ts>
struct dup_tuple_impl<N, T, Ts...> : dup_tuple_impl<N - 1, T, T, Ts...> { };

template<typename T, std::size_t N>
struct dup_tuple {
    using type = typename dup_tuple_impl<N, T>::type;
};

} // namespace detail

template<typename T, std::size_t N>
using dup_tuple = typename detail::dup_tuple<T, N>::type;

} // namespace ash
