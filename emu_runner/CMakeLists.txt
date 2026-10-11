add_executable(emu_runner main.c)

target_link_libraries(emu_runner PRIVATE riscv_model)

target_include_directories(emu_runner
    PRIVATE
        ${CMAKE_SOURCE_DIR}/golden_model
        ${CMAKE_SOURCE_DIR}/golden_model/include
)
# here can change the bin file
target_compile_definitions(emu_runner PRIVATE
    EMU_BIN_PATH="${CMAKE_SOURCE_DIR}/bin/add.bin"
)