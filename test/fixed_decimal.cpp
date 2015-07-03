
#include <Catch/catch.hpp>

#include <ax/decimal.h>
#include <ax/c_utils.h>

using D3 = ax::fixed_decimal<3>;
using D6 = ax::fixed_decimal<6>;
using D8 = ax::fixed_decimal<8>;

TEST_CASE("fixed_decimal_static_asserts", "[fixed_decimal]")
{
    static_assert(D8(1) == D6(1), "Uh oh.");
    static_assert(D8(2) != D6(1), "Uh oh.");
    static_assert(D8(1) < D6(2), "Uh oh.");
    static_assert(D8(2) > D6(1), "Uh oh.");
    static_assert(D8(1) >= D6(1), "Uh oh.");
    static_assert(D8(1) / D8(2) == D8(0.5), "Noo.");
    static_assert(D3(1).as_llong() == 1'000, "Uh oh.");
    static_assert(D6(30).as_llong() == 30'000'000, "Uh oh.");
    static_assert(D6::from_llong(8'000'000) == D6(8), "from_llong");
    static_assert(D6::from_llong(8'000'000) == 8, "from_llong");
    static_assert((D3(1).as_double() - 1.0) < 0.01, "Uh oh.");
    static_assert(D8(D6(1)) == D6(1), "Uh oh.");
    static_assert(D8(2) * 3 == D6(6), "No mul.");
    static_assert(D3(1, 2) == D8(0.5), "Divide.");
    static_assert(D8(2) / 4 == D6(1, 2), "No mul.");
    static_assert(4 / D8(2) == D6(2), "No mul.");
    static_assert(D8(1) / (D8(1) / D8(1)) == D8(1), "Noo.");
    static_assert(ax::c_exp<D8, 2>()(D8(1)) == D8(1), ":(");
    static_assert(ax::c_exp<D8, -2>()(D8(1)) == D8(1), ":(");
    static_assert(ax::c_exp<D8, -2>()(D8(10)) == D8(1, 100), ":(");
    static_assert(D3::from_string("1.234") == D6(1.234), "from_string");
    static_assert(D3::from_string("1.234567") == D6(1.234), "from_string");
    static_assert(D8::from_string("1.234567") == D8(1.234567), "from_string");
}
