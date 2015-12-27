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

#include <Catch/catch.hpp>

#include <ash/tmp_buffer.h>

TEST_CASE("unique tmp buffer constructor", "[tmp_buffer, constructor]")
{
    using buffer = ash::unique_tmp_buffer<int>;
    SECTION("default")
    {
        buffer b{1};
        if (b.size() != 0) {
            CHECK(b.size() != 0);
            CHECK(b.get() != nullptr);
            CHECK(static_cast<bool>(b));
        }
    }

    SECTION("move")
    {
        buffer b{1};
        if (b.size() != 0) {
            buffer b2{std::move(b)};
            CHECK(b2.size() == 1);
            CHECK(b2.get() != nullptr);
        }
        CHECK(b.size() == 0);
        CHECK(b.get() == nullptr);
    }
}

TEST_CASE("unique tmp buffer iteration", "[tmp_buffer]")
{
    using buffer = ash::unique_tmp_buffer<int>;
    buffer b{4};
    if (b.size() != 0) {
        int n{0};
        for (auto i : b) {
            ++n;
        }
        CHECK(b.size() == n);
    }
}

