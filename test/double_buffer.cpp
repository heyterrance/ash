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

#include <string>

#include <Catch/catch.hpp>

#include <ash/double_buffer.h>

TEST_CASE("read lock", "[double_buffer]")
{
    using buffer_type = ash::double_buffer<std::string>;
    buffer_type buf;
    buf.write("hello");
    {
        buffer_type::read_lock rll(buf);
        REQUIRE(rll);
        CHECK(rll.get() == "hello");
    }
    buf.write("world");
    buf.emplace("other");
    std::string dest;
    REQUIRE(buf.try_read(dest));
    CHECK(dest == "other"); // Only get latest data.
    CHECK_FALSE(buf.try_read(dest));

    buf.emplace(3, 'a');
    auto rl = buf.make_read_lock();
    REQUIRE(rl);
    buf.emplace(3, 'b');
    buf.emplace(3, 'c');
    CHECK(rl.get() == "aaa");
}
