include_directories(${AX_INCLUDE_PATH})

add_executable(
    ash_test
    main.cpp
    fixed_decimal.cpp
    fixed_string.cpp
    memory_pool.cpp
)
