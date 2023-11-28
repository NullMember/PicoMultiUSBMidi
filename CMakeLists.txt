cmake_minimum_required(VERSION 3.16)

include(pico_sdk_import.cmake)

project(midi C CXX ASM)

pico_sdk_init()

file(GLOB MIDI_CPP_FILES "${PROJECT_SOURCE_DIR}/src/*.cpp" "${PROJECT_SOURCE_DIR}/src/*.cpp")
file(GLOB MIDI_C_FILES "${PROJECT_SOURCE_DIR}/src/*.cpp" "${PROJECT_SOURCE_DIR}/src/*.c")

add_executable(midi "${MIDI_CPP_FILES}" "${MIDI_C_FILES}")

target_include_directories(midi PRIVATE "${PROJECT_SOURCE_DIR}/include")

target_link_libraries(midi pico_stdlib tinyusb_device tinyusb_board)

pico_enable_stdio_usb(midi 0)
pico_enable_stdio_uart(midi 0)

pico_add_extra_outputs(midi)