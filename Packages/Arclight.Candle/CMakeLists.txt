#
#	 Copyright (c) 2024 - Arclight Team
#
#	 This file is part of Arclight. All rights reserved.
#
#	 CMakeLists.txt
#

project (Arclight.Candle)


file(GLOB_RECURSE AppSources RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp)

add_library(Arclight.Candle STATIC ${AppSources})
target_include_directories(Arclight.Candle PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Include)
target_link_libraries(Arclight.Candle PUBLIC Arclight.Core)

set_target_properties(Arclight.Candle PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Bin/$<CONFIG>
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Bin/$<CONFIG>
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Bin/$<CONFIG>
)
