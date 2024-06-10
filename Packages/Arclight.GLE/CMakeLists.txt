#
#	 Copyright (c) 2024 - Arclight Team
#
#	 This file is part of Arclight. All rights reserved.
#
#	 CMakeLists.txt
#

project (Arclight.GLE)


file(GLOB_RECURSE AppSources RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp)

add_library(Arclight.GLE STATIC ${AppSources})
target_include_directories(Arclight.GLE PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Include)
target_link_libraries(Arclight.GLE PUBLIC Arclight.Core)

LuminaInit(Arclight.GLE ${ArclightLuminaPlatform})
LuminaAddPackage(Arclight.GLE GLEW Linkage Dynamic)

set_target_properties(Arclight.GLE PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Bin/$<CONFIG>
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Bin/$<CONFIG>
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Bin/$<CONFIG>
)