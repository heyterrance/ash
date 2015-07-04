include_directories(${AX_INCLUDE_PATH})

add_executable(
    ash_test
    main.cpp
    data_region.cpp
    fixed_decimal.cpp
    fixed_string.cpp
    memory_pool.cpp
)
