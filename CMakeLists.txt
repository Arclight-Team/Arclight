cmake_minimum_required (VERSION 3.12)

project (arclight)

#Enable C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

#Set output directories
set(BINARY_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BINARY_OUTPUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BINARY_OUTPUT_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BINARY_OUTPUT_DIR})

message("Building as: ${CMAKE_BUILD_TYPE}")

#Profiling via debug symbols
if(MSVC AND CMAKE_CONFIGURATION_TYPES MATCHES "Release")
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
		set(${CompilerFlag} "${${CompilerFlag}} /MP")
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

#Add all include directories
include_directories("src")
include_directories("lib/FMOD/include")
include_directories("lib/GLFW/include")
include_directories("lib/stb_image")
include_directories("lib/assimp/include")
include_directories("lib/bullet3/include/bullet")
include_directories("lib")
include_directories(${GLEW_INCLUDE_DIR})

#Automatically collect sources
file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "src/*.cpp")

#Create the executable and link
add_executable (arclight ${SOURCES})
target_link_libraries(arclight GLEW::GLEW opengl32 glfw 
				${CMAKE_CURRENT_SOURCE_DIR}/lib/FMOD/fmodL_vc.lib ${ASSIMP_LIBRARY} ${BULLET_LIBRARIES})