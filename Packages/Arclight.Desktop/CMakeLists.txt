#
#	 Copyright (c) 2024 - Arclight Team
#
#	 This file is part of Arclight. All rights reserved.
#
#	 CMakeLists.txt
#

project (Arclight.Desktop)


if (ArclightPlatform STREQUAL Windows)

    file(GLOB_RECURSE AppSources RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp)

    add_library(Arclight.Desktop STATIC ${AppSources})
    target_include_directories(Arclight.Desktop PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Include)
    target_link_libraries(Arclight.Desktop PUBLIC Arclight.Core)

    LuminaInit(Arclight.Desktop ${ArclightLuminaPlatform})
    LuminaAddPackage(Arclight.Desktop GLEW Linkage Dynamic)

    set_target_properties(Arclight.Desktop PROPERTIES
            ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Bin/$<CONFIG>
            LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Bin/$<CONFIG>
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Bin/$<CONFIG>
    )

endif ()