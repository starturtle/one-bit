find_package( Qt5 REQUIRED COMPONENTS Core Gui Qml Quick Network )
find_package(Qt5QuickCompiler)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
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
qtquick_compiler_add_resources( RESOURCES resources/stixelator-resources.qrc )

if(DOCTEST_INCLUDE_DIR)
  message(STATUS "build qtgui tests")
  add_executable( test_qtpixelator QtPixelator.cpp )
  target_include_directories( test_qtpixelator PRIVATE ${Qt5_DIR} "${DOCTEST_INCLUDE_DIR}" )
  target_include_directories( test_qtpixelator PUBLIC ${CMAKE_SOURCE_DIR}/utilities )
  target_link_libraries( test_qtpixelator PUBLIC Qt5::Core Qt5::Gui utilities )
  target_compile_definitions( test_qtpixelator PRIVATE -DDOCTEST_CONFIG_IMPLEMENT_WITH_MAIN )
  
  add_executable( test_source_image SourceImage.cpp  ${RESOURCES} )
  target_include_directories( test_source_image PRIVATE ${Qt5_DIR} "${DOCTEST_INCLUDE_DIR}" )
  target_include_directories( test_source_image PUBLIC ${CMAKE_SOURCE_DIR}/utilities )
  target_link_libraries( test_source_image PUBLIC Qt5::Gui Qt5::Quick utilities )
  target_compile_definitions( test_source_image PRIVATE -DDOCTEST_CONFIG_IMPLEMENT_WITH_MAIN )
  set(TEST_IMAGE_DIR test_source_image)
  file(MAKE_DIRECTORY ${TEST_IMAGE_DIR})
  set(TEST_FILE1 schneider_wibbel.jpg)
  set(TEST_FILE2 cow.jpg)
  set(TEST_FILE3 landschaftspark_duisburg_nord.jpg)
  set(TEST_FILE4 tumbled_egg.jpg)
  set(TEST_FILE5 base_stixel.png)
  configure_file(resources/image/${TEST_FILE1} ${TEST_IMAGE_DIR}/${TEST_FILE1} COPYONLY)
  configure_file(resources/image/${TEST_FILE2} ${TEST_IMAGE_DIR}/${TEST_FILE2} COPYONLY)
  configure_file(resources/image/${TEST_FILE3} ${TEST_IMAGE_DIR}/${TEST_FILE3} COPYONLY)
  configure_file(resources/image/${TEST_FILE4} ${TEST_IMAGE_DIR}/${TEST_FILE4} COPYONLY)
  configure_file(resources/image/${TEST_FILE5} ${TEST_IMAGE_DIR}/${TEST_FILE5} COPYONLY)
  target_compile_definitions( test_source_image PRIVATE -DTEST_DIR_NAME="${CMAKE_CURRENT_BINARY_DIR}/${TEST_IMAGE_DIR}/")
  target_compile_definitions( test_source_image PRIVATE -DTEST_FILE_NAME="${CMAKE_CURRENT_BINARY_DIR}/${TEST_IMAGE_DIR}/${TEST_FILE1}")
  target_compile_definitions( test_source_image PRIVATE -DTEST_FILE_NAME_2="${CMAKE_CURRENT_BINARY_DIR}/${TEST_IMAGE_DIR}/${TEST_FILE2}")
  target_compile_definitions( test_source_image PRIVATE -DTEST_FILE_NAME_3="${CMAKE_CURRENT_BINARY_DIR}/${TEST_IMAGE_DIR}/${TEST_FILE3}")
  target_compile_definitions( test_source_image PRIVATE -DTEST_FILE_NAME_4="${CMAKE_CURRENT_BINARY_DIR}/${TEST_IMAGE_DIR}/${TEST_FILE4}")
  target_compile_definitions( test_source_image PRIVATE -DTEST_FILE_NAME_5="${CMAKE_CURRENT_BINARY_DIR}/${TEST_IMAGE_DIR}/${TEST_FILE5}")
endif()