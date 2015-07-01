// Compiletime functions.
//

#pragma once

#include "c_utils/c_pow.h"

namespace ax {

static_assert(c_pow<int, 2, 8>::value == (1 << 8), "Incorrect 2^8");
static_assert(c_pow<int, -4, 3>::value == -64, "Incorrect -4^3");

} // namespace ax
