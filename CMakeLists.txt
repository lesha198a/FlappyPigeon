cmake_minimum_required(VERSION 3.12)

# set(CMAKE_PREFIX_PATH "C:\\Users\\Alexey\\Desktop\\SDL3-3.2.20\\cmake")
# set(CMAKE_PREFIX_PATH "U:\\SDL3-3.2.20\\cmake")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(MyProject VERSION 1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
find_package(SDL3 REQUIRED)

add_executable(main main.c)
add_executable(pointer pointer.c)
add_executable(text string.c)
add_executable(matrix sort.c)
add_executable(3d sortMatrix.c)
add_executable(graphic WIN32 ua.c)
add_executable(graphics WIN32 sdl.c)

target_link_libraries(graphic PRIVATE SDL3::SDL3)
target_include_directories(graphic PUBLIC ${SDL3_INCLUDE_DIRS})
target_compile_definitions(graphic PRIVATE SDL_MAIN_USE_CALLBACKS)

target_link_libraries(graphics PRIVATE SDL3::SDL3)
target_include_directories(graphics PUBLIC ${SDL3_INCLUDE_DIRS})
target_compile_definitions(graphics PRIVATE SDL_MAIN_USE_CALLBACKS)