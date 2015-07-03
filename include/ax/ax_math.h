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

static_assert(num_digits(100, 10) == 3, "num_digits(100, 10) != 3");

}
