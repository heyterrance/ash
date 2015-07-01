
#include <iostream>

#include <ax/c_utils.h>
#include <ax/decimal.h>
#include <ax/free_list.h>
#include <ax/memory_pool.h>

struct elem : ax::free_list_node<elem> { };

void test_free_list()
{
    elem x;
    ax::free_list<elem> list;
    list.add(&x);
    auto* y = list.try_get();
    assert(&x == y);
}

struct pooled : ax::memory_pooled<pooled>
{
    pooled()
    {
        std::cout << "<<< Constructor >>>\n";
    }
    ~pooled()
    {
        std::cout << "<<< Destructor >>>\n";
    }
};

void test_pool()
{
    for (unsigned i = 0; i != 4; ++i) {
        std::unique_ptr<pooled> p(new pooled);
    }
}

int main()
{
    using namespace ax;
    using D3 = fixed_decimal<3>;
    using D6 = fixed_decimal<6>;
    using D8 = fixed_decimal<8>;
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
    static_assert(D3(1).as_double() == 1.0, "Uh oh.");
    static_assert(D8(D6(1)) == D6(1), "Uh oh.");
    static_assert(D8(2) * 3 == D6(6), "No mul.");
    static_assert(D3(1, 2) == D8(0.5), "Divide.");
    static_assert(D8(2) / 4 == D6(1, 2), "No mul.");
    static_assert(4 / D8(2) == D6(2), "No mul.");
    static_assert(D8(1) / (D8(1) / D8(1)) == D8(1), "Noo.");
    static_assert(c_exp<D8, 2>()(D8(1)) == D8(1), ":(");
    static_assert(c_exp<D8, -2>()(D8(1)) == D8(1), ":(");
    static_assert(c_exp<D8, -2>()(D8(10)) == D8(1, 100), ":(");
    static_assert(D3::from_string("1.234") == D6(1.234), "from_string");
    static_assert(D3::from_string("1.234567") == D6(1.234), "from_string");
    static_assert(D8::from_string("1.234567") == D8(1.234567), "from_string");

    test_free_list();
    test_pool();
}
