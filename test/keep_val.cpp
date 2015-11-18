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

#include <ash/keep_val.h>

TEST_CASE("keep boolean", "[keep_val]")
{
    SECTION("true") {
        ash::keep_true kt;
        CHECK_FALSE(kt);
        kt = true;
        CHECK(kt);
        kt = false;
        CHECK(kt);
    }

    SECTION("false") {
        ash::keep_false kf;
        CHECK(kf);
        kf = false;
        CHECK_FALSE(kf);
        kf = true;
        CHECK_FALSE(kf);
    }
}

TEST_CASE("keep integer", "[keep_val]")
{
    ash::keep_int<1> k(0);
    SECTION("increment") {
        CHECK_FALSE(k == 1);
        ++k;
        CHECK(k == 1);
        ++k;
        CHECK(k == 1);
    }

    SECTION("decrement") {
        k = 2;
        CHECK(k == 2);
        CHECK_FALSE(k == 1);
        --k;
        CHECK(k == 1);
        --k;
        CHECK(k == 1);
    }
}
