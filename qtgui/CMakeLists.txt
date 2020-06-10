find_package( Qt5 REQUIRED COMPONENTS Core Gui Qml Quick Network )
find_package(Qt5QuickCompiler)
set(CMAKE_AUTOMOC ON)
add_library( qtgui
  QtPixelator.h
  QtPixelator.cpp
  UiApplication.cpp
  UiApplication.h
  ResultImage.h
  ResultImage.cpp
)
target_include_directories(qtgui PRIVATE  ${Qt5_DIR})
target_link_libraries( qtgui PUBLIC Qt5::Core Qt5::Gui Qt5::Qml Qt5::Quick Qt5::Network )
target_include_directories( qtgui PUBLIC ${CMAKE_SOURCE_DIR}/utilities )
target_link_libraries( qtgui PUBLIC utilities )
qtquick_compiler_add_resources(RESOURCES resources/one-bit-resources.qrc)