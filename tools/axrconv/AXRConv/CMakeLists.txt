cmake_minimum_required(VERSION 3.5)

project(AXRConv LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ASSIMP_BUILD_TESTS OFF)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.
# They need to be set before the find_package(Qt5 ...) call.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(AXRConv
	main.cpp
	mainwindow.cpp
	mainwindow.h
	mainwindow.ui
	importdialog.cpp
	importdialog.h
	importdialog.ui
	importer.cpp
	importer.h
	importconfiguration.h
	amdmodel.h
	types.h
	entitytreeitem.cpp
	entitytreeitem.h
	entitytreemodel.cpp
	entitytreemodel.h
	renderwidget.cpp
	renderwidget.h
	rendercamera.cpp
	rendercamera.h
	axrshader.cpp
	axrshader.h
	resources.qrc
)

add_subdirectory(assimp)
include_directories(assimp/include)

target_link_libraries(AXRConv PRIVATE Qt5::Widgets assimp)
