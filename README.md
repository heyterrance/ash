# A Set of Headers
A handful of useful header-only classes.

## Usage

### `ash::keep_val`

```cpp
template<typename T, T Value>
class keep_val
```
Once `Value` is stored it cannot change.

```cpp
#include <ash/keep_val.h>

ash::keep_int<8> k{0};
while (++k != 8) continue;
assert(k == 8);
assert(++k == 8);

keep_true kt;
assert(!kt);
kt = true;
assert(kt);
kt = false;
assert(kt);
```

### `ash::fixed_decimal`

```cpp
template<unsigned char Precision>
class fixed_decimal
```
A real number precise to `Precision` decimal places.

```cpp
#include <ash/fixed_decimal.h>

ash::fixed_decimal<3> n{50.7};
std::cout << (3 * n) << std::cout; // Prints "152.1"

auto e = ash::fixed_decimal<9>::from_string("2.718281828");
std::cout << e << std::cout; // Prints "2.718281828"
```

### `ash::fixed_string`

```cpp
template<std::size_t Capacity, typename CharT = char>
class fixed_string
```
A char array of size `Capacity` partially implementing the `std::basic_string`
interface. Guarantees null-termination unless the maximum capacity of
characters is reached.

```cpp
#include <ash/fixed_string>

ash::fixed_string<20> hi{"Hello, "};
hi.append("World").append('!');

assert(hi.find("ello") == 1);
assert(hi.find("Hola") == ash::fixed_string<20>::npos);
```

### `ash::memory_pooled`

```cpp
template<class Base>
class memory_pooled
```
Saves any `new`'d instance of `T` when it is deleted, allowing the memory to be
reused.

```cpp
#include <ash/memory_pool.h>

struct Abc : ash::memory_pooled<Abc> { };

Abc* a = std::make_unqiue<Abc>().get();
Abc* b = std::make_unique<Abc>().get();
assert(a == b);

Abc::fill_pool(800); // Add 800 Abcs to the pool.
```

### `ash::stable_storage` and `ash::stable_chunk`

```cpp
template<typename T, std::size_t ChunkSize>
class stable_storage

template<typename T, std::size_t Capacity>
class stable_chunk
```

Create objects with an unchanging position in memory.
`stable_chunk` is a fixed sized region of uninitialized storage. Calls
destructor for all created elements. `stable_storage` uses `stable_chunk` to
implement lists of uninitialized storage.

```cpp
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

```cpp
template<typename T>
class double_buffer
```

A thread-safe, single producer single consumer double buffer. Data may always
be written to the buffer. Only the latest data may be read.

```cpp
#include <ash/double_buffer.h>

ash::double_buffer<std::string> buf;
buf.write("Hello, World");
{
    // Locks reading on the buffer, releases on destruction.
    auto rl = buf.make_read_lock();
    // The following writes don't overrite what's contained in the read_lock.
    buf.write("Hello, Seattle");
    buf.write("G'day, Sydney.");
    assert(rl.get() == "Hello, World.");
}
std::string dest;
bool new_data = buf.try_read(dest);
assert(new_data);
assert(dest == "G'day, Sydney");
```
### `ash::optimistic_buffer`

```cpp
template<typename T, std::size_t NPages = 2>
class optimistic_buffer
```

A thread-safe, single producer, multiple consumer N-buffer. Data may always be
written to the buffer. Readers receive the latest data, which may be invalid.

```cpp
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

```cpp
template<typename>
class function_ptr; // Undefined.

template<typename R, typename... Args>
class function_ptr<R(Args...)>
```
Function pointer wrapper with similar interface to `std::function`.

```cpp
#include <ash/function_ptr.h>

auto mul2 = [](int x){ return x * 2; };
ash::function_ptr<int(int)> f{mul2};

assert(f(5) == mul2(5));
```

### `ash::multipart`
A container of pointers for each type. Allows related data to be contained in non-contingous blocks of memory.

```cpp
template<class... Types> multipart
```

```cpp
// Default construct each type.
ash::multipart<std::string, int, char> mp1;

// Initialize each type to nullptr.
ash::multipart<int, char, double> mp2;

mp2.make<int>(2001); // Allocate and intialize the integer.

assert(mp2.get<int>() == 2001);

// Other types are still null.
assert(mp2.get_ptr<double>() == nullptr);
```

### `ash::unique_tmp_buffer`
RAII wrapper for `std::get_temporary_buffer` and `std::return_temporary_buffer`.

```cpp
template<class T> unique_tmp_buffer
```

```cpp
// Temporary buffer of up to 80 floats.
ash::unique_tmp_buffer<float> buffer{80};

std::fill(buffer.begin(), buffer.end(), 0.0f);

assert(buffer.size() == std::distance(buffer.begin(), buffer.end()));

// Move constructor.
ash::unique_tmp_buffer other{std::move(buffer)};
assert(buffer.data() == nullptr);
```

## Authors
Terrance Howard <heyterrance@gmail.com>
