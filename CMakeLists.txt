cmake_minimum_required (VERSION 3.8)

project (arclight)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

set(BINARY_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BINARY_OUTPUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BINARY_OUTPUT_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BINARY_OUTPUT_DIR})

add_subdirectory("lib/GLFW")

list(APPEND CMAKE_PREFIX_PATH "lib/GLEW")
find_package(GLEW REQUIRED)

include_directories("src")
include_directories("lib/FMOD/include")
include_directories("lib/GLFW/include")
include_directories("lib/stb_image")
include_directories("lib/assimp/include")
include_directories(${GLEW_INCLUDE_DIR})

file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "src/*.cpp")

add_executable (arclight ${SOURCES})
target_link_libraries(arclight GLEW::GLEW opengl32 glfw ${CMAKE_CURRENT_SOURCE_DIR}/lib/FMOD/fmodL_vc.lib ${CMAKE_CURRENT_SOURCE_DIR}/lib/assimp/lib/Release/assimp-vc142-mt.lib)