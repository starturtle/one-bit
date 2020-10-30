cmake_minimum_required(VERSION 3.10)
project( stixelator CXX)

# find dependencies
find_package( OpenCV )

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

if(NOT OpenCV_FOUND AND NOT Qt5_FOUND)
  message(FATAL_ERROR You need either Qt5 for GUI mode or OpenCV for shell mode!)
endif()

# testing
if(DOCTEST_INCLUDE_DIR)
  message(STATUS "enable test projects")
  enable_testing()
endif()

# C++ settings
set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED True )

# subdirectories
add_subdirectory( utilities )
add_subdirectory( imaging )
if( ${USE_QT5} )
  message(STATUS "build with GUI mode")
  set(CMAKE_AUTOMOC ON)
  add_subdirectory( qtgui )
  qtquick_compiler_add_resources(RESOURCES qtgui/resources/one-bit-resources.qrc)
endif()
# executable setup
add_executable( stixelator main.cpp script_mode.h script_mode.cpp ${RESOURCES})
if(OpenCV_FOUND)
  target_include_directories( stixelator PUBLIC ${OpenCV_INCLUDE_DIRS})
  target_link_libraries( stixelator PUBLIC ${OpenCV_LIBS} )
endif()
target_link_libraries( stixelator PUBLIC imaging utilities )
if( ${USE_QT5} )
  target_include_directories( stixelator PUBLIC ${Qt5_DIR} )
  target_link_libraries( stixelator PUBLIC qtgui Qt5::Core Qt5::Gui Qt5::Qml Qt5::Quick Qt5::Network )
endif()


