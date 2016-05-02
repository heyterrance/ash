include_directories(${ASH_INCLUDE_PATH})

add_executable(
    ash_test
    main.cpp
    double_buffer.cpp
    dup_tuple.cpp
    fixed_decimal.cpp
    fixed_string.cpp
    function_ptr.cpp
    keep_val.cpp
    memory_pool.cpp
    multipart.cpp
    optimistic_buffer.cpp
    sstorage.cpp
    tmp_buffer.cpp
)
