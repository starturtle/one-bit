cmake_minimum_required(VERSION 3.10)
project( one_bit CXX)

# find dependencies
find_package( OpenCV REQUIRED )
find_package( Qt5 COMPONENTS Core Gui Qml Quick Network )
if( Qt5_FOUND )
  find_package(Qt5QuickCompiler)
  set(USE_QT5 TRUE)
  set(CMAKE_AUTORCC ON)
  set(CMAKE_AUTOUIC ON)
  add_compile_definitions(USE_QT5)
else()
  set(USE_QT5 FALSE)
endif()

# C++ settings
set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED True )
# subdirectories
add_subdirectory( utilities )
add_subdirectory( imaging )
if( ${USE_QT5} )
  set(CMAKE_AUTOMOC ON)
  add_subdirectory( qtgui )
  qtquick_compiler_add_resources(RESOURCES qtgui/resources/one-bit-resources.qrc)
endif()
# executable setup
add_executable( one_bit main.cpp script_mode.h script_mode.cpp ${RESOURCES})

target_include_directories( one_bit PUBLIC ${OpenCV_INCLUDE_DIRS} ${DOCTEST_INCLUDE_DIR})
target_link_libraries( one_bit PUBLIC imaging utilities ${OpenCV_LIBS} )
if( ${USE_QT5} )
  target_include_directories( one_bit PUBLIC ${Qt5_DIR} )
  target_link_libraries( one_bit PUBLIC qtgui Qt5::Core Qt5::Gui Qt5::Qml Qt5::Quick Qt5::Network )
endif()

enable_testing()
add_test( NAME unitTest COMMAND one_bit_test )
add_executable( one_bit_test test/test_main.cpp ${RESOURCES})

target_include_directories( one_bit_test PUBLIC ${OpenCV_INCLUDE_DIRS} ${DOCTEST_INCLUDE_DIR})
target_link_libraries( one_bit_test PUBLIC imaging_test utilities_test ${OpenCV_LIBS} )
if( ${USE_QT5} )
  target_include_directories( one_bit_test PUBLIC ${Qt5_DIR} )
  target_link_libraries( one_bit_test PUBLIC qtgui_test Qt5::Core Qt5::Gui Qt5::Qml Qt5::Quick Qt5::Network )
endif()

