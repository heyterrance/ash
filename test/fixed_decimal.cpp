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

#include <iostream>
#include <sstream>

#include <Catch/catch.hpp>

#include <ash/fixed_decimal.h>
#include <ash/c_utils.h>

using D3 = ash::fixed_decimal<3>;
using D6 = ash::fixed_decimal<6>;
using D8 = ash::fixed_decimal<8>;

TEST_CASE("construction", "[fixed_decimal]")
{
    SECTION("number of decimal places") {
        CHECK(D3(1).as_llong() == 1'000);
        CHECK(D6(30).as_llong() == 30'000'000);
        CHECK(D3(1).as_double() == Approx(1.0));
    }
    SECTION("rational") {
        CHECK(D3(1, 2) == D8(0.5));
        CHECK(D3(1, 2).as_double() == Approx(0.5));
        CHECK(D3(-3, 10).as_double() == Approx(-0.3));
    }
}

TEST_CASE("comparison", "[fixed_decimal]")
{
    CHECK(D8(1) == D6(1));
    CHECK(D8(2) != D6(1));
    CHECK(D8(D6(1)) == D6(1));
    CHECK(D8(1) < D6(2));
    CHECK(D8(1) <= D6(1));
    CHECK(D8(2) > D6(1));
    CHECK(D8(1) >= D6(1));
}

TEST_CASE("operations", "[fixed_decimal]")
{
    SECTION("multiplication") {
        CHECK(D8(2) * 3 == D6(6));
    }
    SECTION("division") {
        CHECK(D8(1) / D8(2) == D8(0.5));
        CHECK(D8(2) / 4 == D6(1, 2));
        CHECK(4 / D8(2) == D6(2));
        CHECK(D8(1) / (D8(1) / D8(1)) == D8(1));
    }
    SECTION("ash::c_exp") {
        ash::c_exp<D8, -2> neg2_exp;
        ash::c_exp<D8, +2> pos2_exp;
        CHECK(pos2_exp(D8(1)) == D8(1));
        CHECK(neg2_exp(D8(1)) == D8(1));
        CHECK(neg2_exp(D8(10)) == D8(1, 100));
    }
}

TEST_CASE("static creations", "[fixed_decimal]")
{
    SECTION("from_llong") {
        CHECK(D6::from_llong(8'000'000) == D6(8));
        CHECK(D6::from_llong(8'000'000) == 8);
    }
    SECTION("from_string") {
        CHECK(D3::from_string("1.234") == D6(1.234));
        CHECK(D3::from_string("1.234567") == D6(1.234));
        CHECK(D8::from_string("1.234567") == D8(1.234567));
        CHECK(D8::from_string("-0.5") == -D8(0.5));
        CHECK(D8::from_string("-0.5").as_double() == Approx(-0.5));
        SECTION("too many decimal places") {
            CHECK(D3::from_string("1.0005") == 1);
            CHECK_FALSE(D3::from_string("1.005") == 1);
            CHECK(D3::from_string("1.005").as_double() == Approx(1.005));
        }
    }
}

TEST_CASE("to string", "[fixed_decimal]")
{
    std::stringstream ss;
    SECTION("3 decimals") {
        SECTION("no decimal") {
            ss << D3::from_string("123");
            CHECK(ss.str() == "123.000");
        }
        SECTION("some decimals") {
            ss << D3::from_string("1.005");
            CHECK(ss.str() == "1.005");
        }
        SECTION("negative") {
            ss << D3::from_string("-1.305");
            CHECK(ss.str() == "-1.305");
        }
    }
}
