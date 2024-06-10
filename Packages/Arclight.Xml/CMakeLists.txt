#
#	 Copyright (c) 2024 - Arclight Team
#
#	 This file is part of Arclight. All rights reserved.
#
#	 CMakeLists.txt
#

project (Arclight.Crypto)


add_library(Arclight.Xml INTERFACE)
target_include_directories(Arclight.Xml INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/Include)
target_link_libraries(Arclight.Xml INTERFACE Arclight.Core)
