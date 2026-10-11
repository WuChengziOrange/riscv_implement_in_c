set(GOLDEN_MODEL_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cpu.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/debug.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/riscv32_instdef.h
    ${CMAKE_CURRENT_SOURCE_DIR}/peripheral/onboard.h
    ${CMAKE_CURRENT_SOURCE_DIR}/peripheral/result_monitor.h
)

set(GOLDEN_MODEL_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/emu.c
    ${CMAKE_CURRENT_SOURCE_DIR}/peripheral/onboard.c
    ${CMAKE_CURRENT_SOURCE_DIR}/peripheral/result_monitor.c
    ${CMAKE_CURRENT_SOURCE_DIR}/stage/IF.c
    ${CMAKE_CURRENT_SOURCE_DIR}/stage/ID.c
    ${CMAKE_CURRENT_SOURCE_DIR}/stage/EX.c
    ${CMAKE_CURRENT_SOURCE_DIR}/stage/MEM.c
    ${CMAKE_CURRENT_SOURCE_DIR}/stage/WB.c
)

add_library(riscv_model STATIC ${GOLDEN_MODEL_SOURCES} ${GOLDEN_MODEL_HEADERS})

target_include_directories(riscv_model
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(MSVC)
    target_compile_options(riscv_model PRIVATE /W4)
else()
    target_compile_options(riscv_model PRIVATE -Wall -Wextra -Wpedantic)
endif()

option(RISCV_BUILD_DEMO "Build a demo executable for the RISC-V model" OFF)
if(RISCV_BUILD_DEMO)
    add_executable(riscv_demo ${CMAKE_CURRENT_SOURCE_DIR}/main.c)
    target_link_libraries(riscv_demo PRIVATE riscv_model)
endif()
