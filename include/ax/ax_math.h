// Random numerical functions
//

namespace ax {

template<class Integer>
constexpr
unsigned num_digits(Integer n, unsigned char base = 10)
{
    unsigned c = 0;
    while (n != 0) {
        n /= base;
        ++c;
    }
    return c;
}

template<class Integer>
constexpr
unsigned first_digit(Integer n)
{
    if (n < 0)
        return first_digit(-n);
    while (n > 9) {
        n /= 10;
    }
    return n;
}

static_assert(num_digits(100, 10) == 3, "num_digits(100, 10) != 3");
static_assert(first_digit(54) == 5, "first_digit(54) != 5");
static_assert(first_digit(-80) == 8, "first_digit(-80) != 8");
}
