#
#	 Copyright (c) 2024 - Arclight Team
#
#	 This file is part of Lumina. All rights reserved.
#
#	 CMakeLists.txt
#

cmake_minimum_required (VERSION 3.21)

project (Arclight)


#######################
#### PROJECT SETUP ####
#######################

	# Define Debug/Release
	add_compile_definitions(ARC_DEBUG=$<CONFIG:Debug>)
	add_compile_definitions(ARC_RELEASE=$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>,$<CONFIG:MinSizeRel>>)

	# Enable C++23
	set(CMAKE_CXX_STANDARD 23)
	set(CMAKE_CXX_EXTENSIONS off)
	set(CMAKE_CXX_STANDARD_REQUIRED true)

	# Enable C99
	set(CMAKE_C_STANDARD 99)
	set(CMAKE_C_EXTENSIONS off)
	set(CMAKE_C_STANDARD_REQUIRED true)

    # Platform
    set(ArclightPlatform Windows)
    set(ArclightArch x64)


######################
### COMPILER SETUP ###
######################

	if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")

		set(MSVC_CXX_BASE_FLAGS "/Zc:preprocessor /MP /permissive- /utf-8 /DWIN32 /D_WINDOWS /EHsc")
		set(MSVC_C_BASE_FLAGS 	"/Zc:preprocessor /MP /permissive- /utf-8 /DWIN32 /D_WINDOWS")

		set(CMAKE_CXX_FLAGS_DEBUG 			"${MSVC_CXX_BASE_FLAGS} /Zi /Ob0 /Od /RTC1")
		set(CMAKE_CXX_FLAGS_RELEASE 		"${MSVC_CXX_BASE_FLAGS} /GS- /O2 /Ob3 /DNDEBUG")
		set(CMAKE_CXX_FLAGS_RELWITHDEBINFO 	"${MSVC_CXX_BASE_FLAGS} /GS- /Zi /O2 /Ob3 /DNDEBUG")
		set(CMAKE_CXX_FLAGS_MINSIZEREL 		"${MSVC_CXX_BASE_FLAGS} /GS- /O1 /Ob1 /DNDEBUG")
		set(CMAKE_C_FLAGS_DEBUG 			${CMAKE_CXX_FLAGS_DEBUG})
		set(CMAKE_C_FLAGS_RELEASE 			${CMAKE_CXX_FLAGS_RELEASE})
		set(CMAKE_C_FLAGS_RELWITHDEBINFO 	${CMAKE_CXX_FLAGS_RELWITHDEBINFO})
		set(CMAKE_C_FLAGS_MINSIZEREL 		${CMAKE_CXX_FLAGS_MINSIZEREL})

		set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /MANIFESTUAC:NO")
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

		message("Compiling with MSVC")

	elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")

		if ("avx2"			IN_LIST BUILD_CPU_EXTENSIONS)
			set(GCC_ARCH_FLAGS "-mavx2")
		elseif ("avx"		IN_LIST BUILD_CPU_EXTENSIONS)
			set(GCC_ARCH_FLAGS "-mavx")
		elseif ("sse4_2"	IN_LIST BUILD_CPU_EXTENSIONS)
			set(GCC_ARCH_FLAGS "-msse4.2")
		elseif ("sse4_1"	IN_LIST BUILD_CPU_EXTENSIONS)
			set(GCC_ARCH_FLAGS "-msse4.1")
		elseif ("ssse3"		IN_LIST BUILD_CPU_EXTENSIONS)
			set(GCC_ARCH_FLAGS "-mssse3")
		elseif ("sse3"		IN_LIST BUILD_CPU_EXTENSIONS)
			set(GCC_ARCH_FLAGS "-msse3")
		elseif ("sse2"		IN_LIST BUILD_CPU_EXTENSIONS)
			set(GCC_ARCH_FLAGS "-msse2")
		elseif ("sse"		IN_LIST BUILD_CPU_EXTENSIONS)
			set(GCC_ARCH_FLAGS "-msse")
		endif()

		if (MINGW)
			set(GCC_WINPTHREAD_FIX "-Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive")
		endif()

		set(GCC_C_BASE_FLAGS	"${GCC_ARCH_FLAGS} ${GCC_WINPTHREAD_FIX} -static-libgcc -finput-charset=UTF-8 -DWIN32 -D_WINDOWS")
		set(GCC_CXX_BASE_FLAGS	"${GCC_C_BASE_FLAGS} -static-libstdc++")

		set(CMAKE_CXX_FLAGS_DEBUG 			"${GCC_CXX_BASE_FLAGS}	-O0 -g") # TODO Stack related options
		set(CMAKE_CXX_FLAGS_RELEASE 		"${GCC_CXX_BASE_FLAGS}	-O3 -DNDEBUG")
		set(CMAKE_CXX_FLAGS_RELWITHDEBINFO 	"${GCC_CXX_BASE_FLAGS}	-O3 -g -DNDEBUG")
		set(CMAKE_CXX_FLAGS_MINSIZEREL 		"${GCC_CXX_BASE_FLAGS}	-Os -DNDEBUG")
		set(CMAKE_C_FLAGS_DEBUG 			"${GCC_C_BASE_FLAGS}	-Og -g")
		set(CMAKE_C_FLAGS_RELEASE 			"${GCC_C_BASE_FLAGS}	-O3 -DNDEBUG")
		set(CMAKE_C_FLAGS_RELWITHDEBINFO 	"${GCC_C_BASE_FLAGS}	-O3 -g -DNDEBUG")
		set(CMAKE_C_FLAGS_MINSIZEREL 		"${GCC_C_BASE_FLAGS}	-Os -DNDEBUG")

		#set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE}")
		#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS_RELEASE}")

		message("Compiling with GCC/MinGW")

	endif()


######################
### PLATFORM SETUP ###
######################

    if (ArclightPlatform STREQUAL Windows)
        add_compile_definitions(UNICODE _UNICODE)
    endif ()


######################
# PACKAGE GENERATION #
######################

    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} $ENV{Lumina}/Support/CMake)
    include(Lumina)

    set(ArclightLuminaPlatform ${ArclightArch}-${ArclightPlatform})
    set(LuminaLocalPackagePath ${CMAKE_CURRENT_SOURCE_DIR}/Libraries)

    add_custom_target(Arclight)

    set(ArclightPackages Arclight.Core Arclight.Candle Arclight.Crypto Arclight.Desktop Arclight.System Arclight.GLE Arclight.Json Arclight.Noise Arclight.Spring Arclight.Xml)
    set(ArclightPackageBase ${CMAKE_CURRENT_SOURCE_DIR}/Packages)

    foreach (Package IN LISTS ArclightPackages)

        message("Building '" ${Package} "'")
        add_subdirectory(${ArclightPackageBase}/${Package})

    endforeach ()

    add_dependencies(Arclight ${ArclightPackages})


######################
####### CANDLE #######
######################

	add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/Candle")


######################
### PACKAGE EXPORT ###
######################

    set(AkariOutputPath "${CMAKE_CURRENT_SOURCE_DIR}/Akari/Arclight")
    add_custom_target(AkariExport)

    foreach (_PackageTarget IN LISTS ArclightPackages)
        LuminaExportPackage(AkariExport Akari-${_PackageTarget} "${ArclightPackageBase}/${_PackageTarget}" "${AkariOutputPath}/${_PackageTarget}.akari" 1)
    endforeach ()

    set(AkariTPPackages GLEW)
    set(ArclightTPPackageBase ${CMAKE_CURRENT_SOURCE_DIR}/Libraries)
    set(AkariTPOutputPath "${CMAKE_CURRENT_SOURCE_DIR}/Akari/ThirdParty")

    foreach (_PackageTarget IN LISTS AkariTPPackages)
        LuminaExportPackage(AkariExport Akari-${_PackageTarget} "${ArclightTPPackageBase}/${_PackageTarget}" "${AkariTPOutputPath}/${_PackageTarget}.akari" 1)
	endforeach ()
