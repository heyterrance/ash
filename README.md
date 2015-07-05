# A Set of Headers
A handful of useful header-only classes.

## Usage

### `ash::fixed_decimal<unsigned char Prec>`
A real number precise to `Prec` decimal places.

```
#include <ash/fixed_decimal.h>

auto n = ash::fixed_decimal<4>::from_string("1.23456");
std::cout << n << std::cout; // Prints "1.2345"
```

### `ash::fixed_string<std::size_t Capacity>`
A char array of size `Capacity` partially matching the `std::string` interface. Does not guarantee null-termination.

```
#include <ash/fixed_string>

ash::fixed_string<20> hi("Hello, ");
hi.append("World");
```

### `ash::memory_pooled<typename T>`
Saves any `new`'d instance of `T` when it is deleted, allowing the memory to be reused.

```
#include <ash/memory_pool.h>

struct Abc : ash::memory_pooled<Abc> { };

Abc* a = std::make_unqiue<Abc>().get();
Abc* b = std::make_unique<Abc>().get();
assert(a == b);

Abc::fill_pool(800); // Add 800 Abcs to the pool.
```

## Authors
Terrance Howard <heyterrance@gmail.com>
