set(MY_RISCV_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/peripheral
)

file(GLOB MY_RISCV_SOURCES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/peripheral/*.c"
)

if(NOT MY_RISCV_SOURCES)
    message(FATAL_ERROR "No C sources found in ${CMAKE_CURRENT_SOURCE_DIR}")
endif()

add_library(my_riscv STATIC ${MY_RISCV_SOURCES})

target_include_directories(my_riscv
    PUBLIC
        ${MY_RISCV_HEADERS}
    PRIVATE
        ${CMAKE_SOURCE_DIR}/golden_model/include
)

target_compile_options(my_riscv PRIVATE -Wall -Wextra -Wpedantic
    -Wno-unused-variable
    -Wno-unused-parameter
    -Wno-unused-but-set-variable
)


