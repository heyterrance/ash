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

#include <unordered_map>

#include <Catch/catch.hpp>

#include <ash/fixed_string.h>

using S4 = ash::fixed_string<4, char>;
using S8 = ash::fixed_string<8, char>;
using S16 = ash::fixed_string<16, char>;
using S21 = ash::fixed_string<21, char>;

TEST_CASE("fixed string constexpr", "[fixed_string]")
{
    constexpr S4 s("Hi");
    static_assert(s.capacity() == 4, "Incorrect capacity.");
    static_assert(s.length() == 2, "Incorrect length.");
    static_assert(s.size() == s.length(), "Incorrect size.");
    static_assert(s.end() - s.begin() == s.size(), "Bad iterators.");
}

TEST_CASE("fixed string constructors", "[fixed_string, constructor]")
{
    SECTION("default") {
        S4 def{};
        CHECK(def.length() == 0);
        CHECK(def == "");
    }
    SECTION("char pointer") {
        const char* src = "const char*";
        CHECK(S16{src} == src);
        CHECK(S16(src, 5) == "const");
    }
    SECTION("char array") {
        const char src[] = "const array";
        CHECK(S16{src} == src);
        CHECK(S4{src} == "cons");
    }
    SECTION("repeated char") {
        CHECK(S4(1, 'a') == "a");
        CHECK(S4(5, 'a') == "aaaa");
    }
    SECTION("std::string") {
        const std::string src{"const string"};
        CHECK(S16{src} == src);
    }
}

SCENARIO("fixed string insertions", "[fixed_string]")
{
    GIVEN("an empty fixed_string") {
        S16 s;
        REQUIRE(s.empty());
    }
    GIVEN("a non-empty string") {
        S16 s("123");
        REQUIRE_FALSE(s.empty());
    }
}

TEST_CASE("==", "[fixed_string, comparison]")
{
    ash::fixed_string<8> s{"Hello"};
    SECTION("overloads") {
        CHECK(s == "Hello");
        CHECK(s == "Hello\0\0\0");
        CHECK_FALSE(s == "Hi");
        CHECK_FALSE(s == "HelloO");
        CHECK(s == std::string{"Hello"});
        CHECK_FALSE(s == std::string{"Hi"});
        CHECK(s == (const char*) "Hello");
        CHECK_FALSE(s == (const char*) "HelloHelloHello");
    }
    SECTION("full buffer") {
        s = ash::fixed_string<8>{8, 'a'};
        CHECK(s == std::string(8, 'a'));
    }
}

TEST_CASE("append", "[fixed_string]")
{
    ash::fixed_string<8> s;
    SECTION("sing char") {
        s.append('a');
        CHECK(s.length() == 1);
        CHECK(s == "a");
    }
    SECTION("mutliple char") {
        s.append(1, 'a');
        CHECK(s.length() == 1);
        CHECK(s == "a");
        s.append(2, 'b');
        CHECK(s.length() == 3);
        CHECK(s == "abb");
    }
    SECTION("another fixed_string") {
        s.append("AB");
        CHECK(s == "AB");
        s.append("CD").append("EF");
        CHECK(s == "ABCDEF");
    }
    SECTION("overflow") {
        s.append(7, 'a');
        CHECK(s == std::string(7, 'a'));
        s.append(4, 'a');
        CHECK_FALSE(s == std::string(11, 'a'));
        CHECK(s == std::string(8, 'a'));
    }
    SECTION("operator+=") {
        s += 'a';
        s += 'b';
        CHECK(s == "ab");
    }
}

TEST_CASE("hashable", "[fixed_string, hash]")
{
    std::unordered_map<ash::fixed_string<4>, char> m;
    m.emplace("ABC", 3);
    CHECK(m["ABC"] == 3);
    m.emplace("XYZ", 8);
    CHECK_FALSE(m["ABC"] == 8);
    CHECK(m["XYZ"] == 8);
}

TEST_CASE("swap", "[fixed_string]")
{
    S4 a{"abcd"}, w{"wxyz"};
    std::swap(a, w);
    CHECK(a == "wxyz");
    CHECK(w == "abcd");
}

TEST_CASE("find", "[fixed_string]")
{
    SECTION("single char") {
        const S16 hello{"hello"};
        CHECK(hello.find('l') == 2);
        CHECK(hello.find('l', 3) == 3);
        CHECK(hello.find('h') == 0);
        CHECK(hello.find('h', 1) == S16::npos);
        CHECK(hello.find('x') == S16::npos);
    }
    SECTION("substring") {
        const S16 hello{"hello"};
        CHECK(hello.find("ello") == 1);
        CHECK(hello.find("elloh") == S16::npos);

        const S4 very{"very"};
        CHECK(very.find("very") == 0);
        CHECK(very.find("very", 1) == S4::npos);
    }
}
