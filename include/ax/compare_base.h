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

namespace ax {

template<class Base>
struct compareable
{
    template<class R>
    friend constexpr
    bool operator!=(const Base& lhs, const R& rhs)
    {
        return not (lhs == rhs);
    }

    template<class R>
    friend constexpr
    bool operator>=(const Base& lhs, const R& rhs)
    {
        return not (lhs < rhs);
    }

    template<class R>
    friend constexpr
    bool operator>(const Base& lhs, const R& rhs)
    {
        return not (lhs <= rhs);
    }

    template<class R>
    friend constexpr
    bool operator<=(const Base& lhs, const R& rhs)
    {
        return (lhs < rhs) or (lhs == rhs);
    }
};

} // namespace ax
