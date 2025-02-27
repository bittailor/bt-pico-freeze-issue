# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico2_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(bt-pico-freeze-issue C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(bt-pico-freeze-issue main.cpp )

pico_set_program_name(bt-pico-freeze-issue "bt-pico-freeze-issue")
pico_set_program_version(bt-pico-freeze-issue "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(bt-pico-freeze-issue 1)
pico_enable_stdio_usb(bt-pico-freeze-issue 1)
target_compile_definitions(bt-pico-freeze-issue PRIVATE 
    PICO_STDIO_USB_CONNECT_WAIT_TIMEOUT_MS=2000)

# Add the standard library to the build
target_link_libraries(bt-pico-freeze-issue pico_stdlib)

# Add the standard include files to the build
target_include_directories(bt-pico-freeze-issue PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

# Add any user requested libraries
target_link_libraries(bt-pico-freeze-issue 
    pico_cyw43_arch_none
    pico_btstack_ble
    pico_btstack_cyw43
)

pico_btstack_make_gatt_header(bt-pico-freeze-issue  PRIVATE "${CMAKE_CURRENT_LIST_DIR}/peripheral.gatt")

pico_add_extra_outputs(bt-pico-freeze-issue)

