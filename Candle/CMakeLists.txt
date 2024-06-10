#
#	 Copyright (c) 2024 - Arclight Team
#
#	 This file is part of Arclight. All rights reserved.
#
#	 CMakeLists.txt
#

project (Candle)


file(GLOB_RECURSE AppSources RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp)

add_executable(Candle ${AppSources})
target_link_libraries(Candle PUBLIC ${ArclightPackages})

set_target_properties(Candle PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Bin/$<CONFIG>
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Bin/$<CONFIG>
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Bin/$<CONFIG>
)
