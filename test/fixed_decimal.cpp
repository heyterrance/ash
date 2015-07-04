#include <iostream>
#include <sstream>

#include <Catch/catch.hpp>

#include <ax/fixed_decimal.h>
#include <ax/c_utils.h>

using D3 = ax::fixed_decimal<3>;
using D6 = ax::fixed_decimal<6>;
using D8 = ax::fixed_decimal<8>;

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
    SECTION("ax::c_exp") {
        ax::c_exp<D8, -2> neg2_exp;
        ax::c_exp<D8, +2> pos2_exp;
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
