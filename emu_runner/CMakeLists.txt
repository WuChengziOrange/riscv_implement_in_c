add_executable(emu_runner main.c)

target_link_libraries(emu_runner PRIVATE riscv_model)
target_link_libraries(emu_runner PRIVATE my_riscv)

target_include_directories(emu_runner
    PRIVATE
        ${CMAKE_SOURCE_DIR}/golden_model
        ${CMAKE_SOURCE_DIR}/golden_model/include
        ${CMAKE_SOURCE_DIR}/my_riscv_model
)
# here can change the bin file
target_compile_definitions(emu_runner PRIVATE
    # EMU_BIN_PATH="${CMAKE_SOURCE_DIR}/bin/add.bin"
    EMU_BIN_PATH="${CMAKE_SOURCE_DIR}/source_for_riscv_platform/build/firmware.bin"
)