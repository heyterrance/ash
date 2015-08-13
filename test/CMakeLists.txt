include_directories(${AX_INCLUDE_PATH})
include_directories(${EXT_INCLUDE_PATH})

add_executable(
    ash_test
    main.cpp
    double_buffer.cpp
    fixed_decimal.cpp
    fixed_string.cpp
    function_ptr.cpp
    memory_pool.cpp
    sstorage.cpp
)
