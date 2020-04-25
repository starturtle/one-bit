cmake_minimum_required(VERSION 3.10)
project( one_bit CXX)
# find dependencies
find_package( OpenCV REQUIRED )
find_package( Qt5 REQUIRED COMPONENTS Core Gui Qml Quick Network )
find_package(Qt5QuickCompiler)
# C++ settings
set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED True )
# subdirectories
add_subdirectory( utilities )
add_subdirectory( imaging )
add_subdirectory( gui )
qtquick_compiler_add_resources(RESOURCES gui/resources/one-bit-resources.qrc)
# executable setup
add_executable( one_bit main.cpp script_mode.h script_mode.cpp ${RESOURCES})
target_include_directories( one_bit PUBLIC ${OpenCV_INCLUDE_DIRS} ${Qt5_DIR} )
target_link_libraries( one_bit PUBLIC imaging utilities ${OpenCV_LIBS} )
target_link_libraries( one_bit PUBLIC gui Qt5::Core Qt5::Gui Qt5::Qml Qt5::Quick Qt5::Network )

