# A Set of Headers
A handful of useful header-only classes.

## Usage

### `ash::fixed_decimal`

```
template<unsigned char Precision>
class fixed_decimal
```
A real number precise to `Precision` decimal places.

```
#include <ash/fixed_decimal.h>

ash::fixed_decimal<3> n{50.7};
std::cout << (3 * n) << std::cout; // Prints "152.1"

auto e = ash::fixed_decimal<9>::from_string("2.718281828");
std::cout << e << std::cout; // Prints "2.718281828"
```

### `ash::fixed_string`

```
template<std::size_t Capacity, typename CharT = char>
class fixed_string
```
A char array of size `Capacity` partially implementing the `std::basic_string`
interface. Guarantees null-termination unless the maximum capacity of
characters is reached.

```
#include <ash/fixed_string>

ash::fixed_string<20> hi{"Hello, "};
hi.append("World").append('!');

assert(hi.find("ello") == 1);
assert(hi.find("Hola") == ash::fixed_string<20>::npos);
```

### `ash::memory_pooled`

```
template<class Base>
class memory_pooled
```
Saves any `new`'d instance of `T` when it is deleted, allowing the memory to be
reused.

```
#include <ash/memory_pool.h>

struct Abc : ash::memory_pooled<Abc> { };

Abc* a = std::make_unqiue<Abc>().get();
Abc* b = std::make_unique<Abc>().get();
assert(a == b);

Abc::fill_pool(800); // Add 800 Abcs to the pool.
```

### `ash::stable_storage` and `ash::stable_chunk`

```
template<typename T, std::size_t ChunkSize>
class stable_storage

template<typename T, std::size_t Capacity>
class stable_chunk
```

Create objects with an unchanging position in memory.
`stable_chunk` is a fixed sized region of uninitialized storage. Calls
destructor for all created elements. `stable_storage` uses `stable_chunk` to
implement lists of uninitialized storage.

```
#include <ash/sstorage.h>

ash::stable_chunk<int, 2> schunk;

// There's no way to retrieve objects created by create() or
// try_create() except through the returned reference.
auto& x = schunk.create(8);
schunk.create(); // Lost reference.
try {
    schunk.try_create(2);
} catch (const std::out_of_range& oor) {
    std::cerr << "No more space in chunk." std::endl;
}

// stable_storage has no try_create() call. Another chunk will
// be created once the current one if full.
ash::stable_storage<int, 2> sstore;
while (sstore.chunk_count() != 3) {
    sstore.create();
}
```

### `ash::double_buffer`

```
template<typename T>
class double_buffer
```

A thread-safe, single producer single consumer double buffer. Data may always
be written to the buffer. Only the latest data may be read.

```
#include <ash/double_buffer.h>

ash::double_buffer<std::string> buf;
buf.write("Hello, World");
{
    // Locks reading on the buffer, releases on destruction.
    auto rg = buf.make_read_lock();
    // The following writes don't overrite what's contained in the read_lock.
    buf.write("Hello, Seattle");
    buf.write("G'day, Sydney.");
    assert(rg.get() == "Hello, World.");
}
std::string dest;
bool new_data = buf.try_read(dest);
assert(new_data);
assert(dest == "G'day, Sydney");
```
### `ash::optimistic_buffer`

```
template<typename T, std::size_t NPages = 2>
class optimistic_buffer
```

A thread-safe, single producer, multiple consumer N-buffer. Data may always be
written to the buffer. Readers receive the latest data, which may be invalid.

```
#include <ash/optimistic_buffer.h>

// A 1-page buffer of std::string.
ash::optimistic_buffer<std::string, 1> buf;
std::thread thd([&]{
    buf.write("Hello, World");
    buf.write("Goodbye");
    buf.write_with([](std::string& dst){
        dst.append("!");
    });
});

std::string dest;
bool valid_read = buf.try_read(dest);
if (valid_read) {
    assert(
        dest == "" or // No data written, read default value.
        dest == "Hello, World" or
        dest == "Goodbye" or
        dest == "Goodbye!");
}

// Try 1,200 times to get valid data.
static constexpr unsigned n_retires = 1200;
bool valid_eventually = buf.try_read(dest, n_retries);
```


### `ash::function_ptr`

```
template<typename>
class function_ptr; // Undefined.

template<typename R, typename... Args>
class function_ptr<R(Args...)>
```
Function pointer wrapper with similar interface to `std::function`.

```
#include <ash/function_ptr.h>

auto mul2 = [](int x){ return x * 2; };
ash::function_ptr<int(int)> f{mul2};

assert(f(5) == mul2(5));
```

## Authors
Terrance Howard <heyterrance@gmail.com>
