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
  SourceImage.h
  SourceImage.cpp
)

target_include_directories( qtgui PRIVATE ${Qt5_DIR})
target_link_libraries( qtgui PUBLIC Qt5::Core Qt5::Gui Qt5::Qml Qt5::Quick Qt5::Network )
target_include_directories( qtgui PUBLIC ${CMAKE_SOURCE_DIR}/utilities )
target_link_libraries( qtgui PUBLIC utilities )
qtquick_compiler_add_resources( RESOURCES resources/one-bit-resources.qrc )

if(DOCTEST_INCLUDE_DIR)
  message(STATUS "build qtgui tests")
  add_executable( test_qtpixelator QtPixelator.cpp )
  target_include_directories( test_qtpixelator PRIVATE ${Qt5_DIR} "${DOCTEST_INCLUDE_DIR}" )
  target_include_directories( test_qtpixelator PUBLIC ${CMAKE_SOURCE_DIR}/utilities )
  target_link_libraries( test_qtpixelator PUBLIC Qt5::Core Qt5::Gui utilities )
  target_compile_definitions( test_qtpixelator PRIVATE -DDOCTEST_CONFIG_IMPLEMENT_WITH_MAIN )
  
  add_executable( test_source_image SourceImage.cpp )
  target_include_directories( test_source_image PRIVATE ${Qt5_DIR} "${DOCTEST_INCLUDE_DIR}" )
  target_include_directories( test_source_image PUBLIC ${CMAKE_SOURCE_DIR}/utilities )
  target_link_libraries( test_source_image PUBLIC Qt5::Gui Qt5::Quick utilities )
  target_compile_definitions( test_source_image PRIVATE -DDOCTEST_CONFIG_IMPLEMENT_WITH_MAIN )
endif()