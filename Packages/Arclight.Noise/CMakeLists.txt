#
#	 Copyright (c) 2024 - Arclight Team
#
#	 This file is part of Arclight. All rights reserved.
#
#	 CMakeLists.txt
#

project (Arclight.Noise)


add_library(Arclight.Noise INTERFACE)
target_include_directories(Arclight.Noise INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/Include)
target_link_libraries(Arclight.Noise INTERFACE Arclight.Core)
