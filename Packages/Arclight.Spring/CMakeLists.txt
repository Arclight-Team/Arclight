#
#	 Copyright (c) 2024 - Arclight Team
#
#	 This file is part of Arclight. All rights reserved.
#
#	 CMakeLists.txt
#

project (Arclight.Spring)


file(GLOB_RECURSE AppSources RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp)

add_library(Arclight.Spring STATIC ${AppSources})
target_include_directories(Arclight.Spring PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Include)
target_link_libraries(Arclight.Spring PUBLIC Arclight.Core Arclight.GLE)

set_target_properties(Arclight.Spring PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Bin/$<CONFIG>
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Bin/$<CONFIG>
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Bin/$<CONFIG>
)