# CMakeList.txt : CMake project for NesPower, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

add_definitions(/ZI)

find_package(SDL2 CONFIG REQUIRED)

# Add source to this project's executable.
add_executable (NesPower "main.c"  "include/cpu.h" "src/cpu.c" "include/types.h" "include/ppu.h" "include/mapper.h" "include/controls.h" "include/tracer.h" "src/ppu.c" "src/mapper.c" "src/controls.c" "src/tracer.c")

target_link_libraries(NesPower PRIVATE SDL2::SDL2 SDL2::SDL2main)
