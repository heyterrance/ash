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

#include <utility>
#include <functional>

#include <Catch/catch.hpp>

#include <ash/function_ptr.h>

int g_val = 0;

void set_g(int x) { g_val = x; }
void set_2g(int x) { g_val = 2 * x; }

void set_x(int& dest, int src) { dest = src; }
void set_2x(int& dest, int src) { dest = src * 2; }

TEST_CASE("no return", "[function_ptr]")
{
    SECTION("set global") {
        ash::function_ptr<void(int)> f;
        CHECK_FALSE(f);
        f = set_g;
        f(1);
        REQUIRE(f);
        CHECK(g_val == 1);

        f = set_2g;
        REQUIRE(f);
        f(3);
        CHECK(g_val == 6);
    }

    SECTION("pass by reference") {
        ash::function_ptr<void(int&, int)> f{set_x};
        int dest = 0;
        f(dest, 3);
        CHECK(dest == 3);
        f = set_2x;
        f(dest, 5);
        CHECK(dest == 10);
    }
}

int mul(int a, int b) { return a * b; }
auto add = [](int a, int b){ return a + b; };

TEST_CASE("return value", "[function_ptr]")
{
    ash::function_ptr<int(int, int)> f{mul};
    REQUIRE(f);
    CHECK(f(3, -4) == -12);

    f = add;
    CHECK(f(3, -4) == -1);
}

TEST_CASE("swap and move", "[function_ptr]")
{
    ash::function_ptr<int(int, int)> f{add}, g{mul};
    REQUIRE(f);
    REQUIRE(g);
    CHECK(f(4, 4) == g(1, 8));

    f.swap(g);
    REQUIRE(f); // mul
    REQUIRE(g); // add
    CHECK(f(7, 3) == g(8, 13));

    std::swap(f, g);
    REQUIRE(f); // add
    REQUIRE(g); // mul
    CHECK(f(7, 3) == g(2, 5));

    f = std::move(g);
    REQUIRE(f);
    CHECK_FALSE(g);
    CHECK(f(5, -2) == -10);
}

TEST_CASE("throw", "[function_ptr]")
{
    ash::function_ptr<void()> f;
    CHECK_THROWS_AS(f(), std::bad_function_call);
    f = []{};
    CHECK_NOTHROW(f());
}
