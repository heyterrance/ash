
#include <Catch/catch.hpp>

#include <ax/fixed_string.h>

using S4 = ax::fixed_string<4, char>;
using S8 = ax::fixed_string<8, char>;
using S16 = ax::fixed_string<16, char>;
using S21 = ax::fixed_string<21, char>;

TEST_CASE("fixed string constexpr", "[fixed_string]")
{
    constexpr S4 s("Hi");
    static_assert(s.capacity() == 4, "Incorrect capacity.");
    static_assert(s.length() == 2, "Incorrect length.");
    static_assert(s.size() == s.length(), "Incorrect size.");
    static_assert(s.end() - s.begin() == s.size(), "Bad iterators.");
}

SCENARIO("fixed string insertions", "[fixed_string]")
{
    GIVEN("an empty fixed_string") {
        S16 s;
        REQUIRE(s.empty());

        WHEN("a small string is appended") {
            s.append("a");
            THEN("the string is added") {
                CHECK(s.length() == 1);
            }
        }
        WHEN("two small strings are appened") {
            s.append("a").append("b");
            THEN("the strings are in the correct order") {
                CHECK(s.length() == 2);
                CHECK(s.str() == "ab");
            }
        }
        WHEN("another fixed string is appened") {
            s.append("a").append(S8("bcd"));
            THEN("all the contents are appened") {
                CHECK(s.length() == 4);
                CHECK(s.str() == "abcd");
            }
        }
    }

    GIVEN("a non-empty string") {
        S16 s("123");
        REQUIRE_FALSE(s.empty());
    }
}
