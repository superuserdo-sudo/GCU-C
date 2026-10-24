cmake_minimum_required(VERSION 3.20)
project(GCU VERSION 0.4.0 LANGUAGES C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

option(GCU_NATIVE "Tune the binary for the build machine" ON)
option(GCU_LTO "Enable link-time optimization" ON)
set(GCU_OPT_LEVEL "3" CACHE STRING "Optimization level (0, 1, 2, 3, or s)")

add_executable(gcu
    src/main.c
    src/files.c
    src/text.c
    src/system.c
    src/compat.c
)
target_include_directories(gcu PRIVATE include)
target_compile_options(gcu PRIVATE
    -O${GCU_OPT_LEVEL}
    -Wall -Wextra -Wpedantic -Wconversion -Wshadow
    -D_FORTIFY_SOURCE=3
)
if(GCU_NATIVE)
    target_compile_options(gcu PRIVATE -march=native)
endif()
if(GCU_LTO)
    include(CheckIPOSupported)
    check_ipo_supported(RESULT GCU_IPO_SUPPORTED)
    if(GCU_IPO_SUPPORTED)
        set_property(TARGET gcu PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
    endif()
endif()

file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/commands.txt" GCU_COMMANDS)
list(FILTER GCU_COMMANDS EXCLUDE REGEX "^[ \t]*$")
list(FILTER GCU_COMMANDS EXCLUDE REGEX "^[ \t]*#")

set(GCU_COMMAND_BIN_DIR "${CMAKE_BINARY_DIR}/bin")
file(MAKE_DIRECTORY "${GCU_COMMAND_BIN_DIR}")
foreach(command IN LISTS GCU_COMMANDS)
    add_custom_command(TARGET gcu POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E create_symlink ../gcu "${GCU_COMMAND_BIN_DIR}/${command}"
        VERBATIM
    )
endforeach()

install(TARGETS gcu RUNTIME DESTINATION bin)
install(CODE "file(MAKE_DIRECTORY \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/bin\")")
foreach(command IN LISTS GCU_COMMANDS)
    install(CODE "execute_process(COMMAND \"${CMAKE_COMMAND}\" -E create_symlink gcu \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/bin/${command}\")")
endforeach()
