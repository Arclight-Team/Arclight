cmake_minimum_required (VERSION 3.12)

set(PROJECT_NAME arclight)
project (${PROJECT_NAME})

if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
add_compile_definitions(ARC_DEBUG=true)
add_compile_definitions(ARC_RELEASE=false)
else()
add_compile_definitions(ARC_DEBUG=false)
add_compile_definitions(ARC_RELEASE=true)
endif()

#Enable C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

#Set output directories
set(BINARY_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BINARY_OUTPUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BINARY_OUTPUT_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BINARY_OUTPUT_DIR})

#Set Uri root path
add_definitions(-DARC_URI_ROOT="../../")

#ImGui implementation
set(ARC_BUILD_IMGUI false)
add_compile_definitions(ARC_USE_IMGUI=${ARC_BUILD_IMGUI})

#Discord Game SDK implementation
set(ARC_BUILD_DISCORD FALSE)
add_compile_definitions(ARC_USE_DISCORD=${ARC_BUILD_DISCORD})

message("Building ${PROJECT_NAME} as: ${CMAKE_BUILD_TYPE}")

#Profiling via debug symbols
if(MSVC AND CMAKE_BUILD_TYPE MATCHES "Release")
	message("Profiling active")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
	add_link_options("/DEBUG")
endif()

#Link MSVC runtime statically + parallel build
if(MSVC)
	set(CompilerFlags CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE)
	message("Flags:")
	foreach(CompilerFlag ${CompilerFlags})
		string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
		set(${CompilerFlag} "${${CompilerFlag}} /MP /permissive")
		message(${CompilerFlag} ": ${${CompilerFlag}}")
	endforeach()
	set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif()

#Build GLFW along the project
add_subdirectory("lib/GLFW")

#Find GLEW
list(APPEND CMAKE_PREFIX_PATH "lib/GLEW")
find_package(GLEW REQUIRED)

#Use the correct assimp .lib
if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
	message("Linking assimp in debug mode")
	set(ASSIMP_LIBRARY ${CMAKE_CURRENT_SOURCE_DIR}/lib/assimp/lib/Debug/assimp-vc142-mtd.lib)
else()
	message("Linking assimp in release mode")
	set(ASSIMP_LIBRARY ${CMAKE_CURRENT_SOURCE_DIR}/lib/assimp/lib/Release/assimp-vc142-mt.lib)
endif()

#Use the correct bullet .libs
if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
	message("Linking bullet in debug mode")
	set(BULLET_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/bullet3/lib/Debug)
	set(BULLET_LIBRARIES	${BULLET_LIB_DIR}/BulletCollision_Debug.lib
							${BULLET_LIB_DIR}/BulletDynamics_Debug.lib
							${BULLET_LIB_DIR}/LinearMath_Debug.lib)
else()
	message("Linking bullet in release mode")
	set(BULLET_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/bullet3/lib/Release)
	set(BULLET_LIBRARIES	${BULLET_LIB_DIR}/BulletCollision.lib
							${BULLET_LIB_DIR}/BulletDynamics.lib
							${BULLET_LIB_DIR}/LinearMath.lib)
endif()

#Use double precision for physics simulation (causes a stack smash as of now)
#add_compile_definitions(BT_USE_DOUBLE_PRECISION)


list(APPEND SOURCE_TARGETS "src/*cpp")

#Add all include directories
list(APPEND INCLUDE_TARGETS "src")
list(APPEND INCLUDE_TARGETS "lib")
list(APPEND INCLUDE_TARGETS "lib/FMOD/include")
list(APPEND INCLUDE_TARGETS "lib/GLFW/include")
list(APPEND INCLUDE_TARGETS "lib/stb_image")
list(APPEND INCLUDE_TARGETS "lib/assimp/include")
list(APPEND INCLUDE_TARGETS "lib/bullet3/include/bullet")
list(APPEND INCLUDE_TARGETS ${GLEW_INCLUDE_DIR})

#Add common libraries
list(APPEND LIBRARY_TARGETS GLEW::GLEW)
list(APPEND LIBRARY_TARGETS opengl32)
list(APPEND LIBRARY_TARGETS glfw)
list(APPEND LIBRARY_TARGETS ${CMAKE_CURRENT_SOURCE_DIR}/lib/FMOD/fmodL_vc.lib)
list(APPEND LIBRARY_TARGETS ${ASSIMP_LIBRARY})
list(APPEND LIBRARY_TARGETS ${BULLET_LIBRARIES})

if(${ARC_BUILD_IMGUI})
message("Building ${PROJECT_NAME} with ImGui")
list(APPEND INCLUDE_TARGETS "lib/imgui/include")
list(APPEND INCLUDE_TARGETS "lib/imgui/backends")
list(APPEND SOURCE_TARGETS "lib/imgui/source/*.cpp" "lib/imgui/backends/*.cpp")
endif()

if(${ARC_BUILD_DISCORD})
message("Building ${PROJECT_NAME} with Discord Game SDK")
list(APPEND INCLUDE_TARGETS "lib/DiscordGameSDK/include")
list(APPEND LIBRARY_TARGETS ${CMAKE_CURRENT_SOURCE_DIR}/lib/DiscordGameSDK/lib/x64/${CMAKE_BUILD_TYPE}/discord_game_sdk_cpp.lib)
list(APPEND LIBRARY_TARGETS ${CMAKE_CURRENT_SOURCE_DIR}/lib/DiscordGameSDK/lib/x64/${CMAKE_BUILD_TYPE}/discord_game_sdk.dll.lib)
endif()

#Sources/includes/libraries specified

#Add all include directories
include_directories(${INCLUDE_TARGETS})

#Automatically collect sources
file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_SOURCE_DIR} ${SOURCE_TARGETS})

if(NOT ${ARC_BUILD_IMGUI})
list(FILTER SOURCES EXCLUDE REGEX "src/render/gui/imgui/*")
endif()

#Create the executable and link
add_executable (${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${LIBRARY_TARGETS})