#
#	 Copyright (c) 2024 - Arclight Team
#
#	 This file is part of Arclight. All rights reserved.
#
#	 CMakeLists.txt
#

project (Arclight.System)


if (ArclightPlatform STREQUAL Windows)

    add_library(Arclight.System STATIC)
    target_link_libraries(Arclight.System PUBLIC Arclight.Core)

    set_target_properties(Arclight.System PROPERTIES
            ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Bin/$<CONFIG>
            LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Bin/$<CONFIG>
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Bin/$<CONFIG>
    )

    file(GLOB_RECURSE PlatformSources RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/Source/Platform/${ArclightPlatform}/*.cpp)
    target_sources(Arclight.System PRIVATE ${PlatformSources})

    target_include_directories(Arclight.System PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Include/Platform/${ArclightPlatform})

endif ()
