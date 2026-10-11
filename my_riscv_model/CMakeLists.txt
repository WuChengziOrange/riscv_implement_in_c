set(MY_RISCV_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(MY_RISCV_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.c
)

add_library(my_riscv STATIC ${MY_RISCV_SOURCES})

target_include_directories(my_riscv
    PUBLIC
        ${MY_RISCV_HEADERS}
    PRIVATE
        ${CMAKE_SOURCE_DIR}/golden_model/include
)

target_compile_options(my_riscv PRIVATE -Wall -Wextra -Wpedantic)



