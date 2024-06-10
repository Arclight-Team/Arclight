#
#	 Copyright (c) 2024 - Arclight Team
#
#	 This file is part of Arclight. All rights reserved.
#
#	 CMakeLists.txt
#

project (Arclight.Core)


file(GLOB_RECURSE AppSources RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/Source/Main/*.cpp)

add_library(Arclight.Core STATIC ${AppSources})
target_include_directories(Arclight.Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Include/Main)

set_target_properties(Arclight.Core PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Bin/$<CONFIG>
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Bin/$<CONFIG>
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Bin/$<CONFIG>
)

if (ArclightPlatform STREQUAL Windows)

    file(GLOB_RECURSE PlatformSources RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/Source/Platform/Windows/*.cpp)
    target_sources(Arclight.Core PRIVATE ${PlatformSources})

    target_include_directories(Arclight.Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Include/Platform/Windows)
    target_link_libraries(Arclight.Core PUBLIC ComCtl32.lib Bcrypt.lib)

endif ()