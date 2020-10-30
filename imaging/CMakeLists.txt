find_package( OpenCV REQUIRED )

add_library( pixelator  
  Pixelator.cpp
  Pixelator.h
)
target_include_directories( pixelator PUBLIC ${OpenCV_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/utilities "${DOCTEST_INCLUDE_DIR}" )
target_link_libraries( pixelator PUBLIC ${OpenCV_LIBS} )
target_compile_definitions(pixelator PRIVATE -DDOCTEST_CONFIG_DISABLE)

add_library( imaging  
  ShrinkPixelator.cpp
  ShrinkPixelator.h
)
target_include_directories( imaging PUBLIC ${OpenCV_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/utilities )
target_link_libraries( imaging PUBLIC ${OpenCV_LIBS} pixelator)

if(DOCTEST_INCLUDE_DIR)
  message(STATUS "build imaging tests")
  add_executable( test_pixelator Pixelator.cpp )
  target_include_directories( test_pixelator PUBLIC ${OpenCV_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/utilities "${DOCTEST_INCLUDE_DIR}" )
  target_link_libraries( test_pixelator PUBLIC ${OpenCV_LIBS} )
  target_compile_definitions( test_pixelator PRIVATE -DDOCTEST_CONFIG_IMPLEMENT_WITH_MAIN )

  add_executable( test_shrink_pixelator ShrinkPixelator.cpp )
  target_include_directories( test_shrink_pixelator PUBLIC ${OpenCV_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/utilities "${DOCTEST_INCLUDE_DIR}" )
  target_link_libraries( test_shrink_pixelator PUBLIC ${OpenCV_LIBS} pixelator utilities )
  target_compile_definitions( test_shrink_pixelator PRIVATE -DDOCTEST_CONFIG_IMPLEMENT_WITH_MAIN -DIGNORE_BASE_CLASS_TESTS)
endif()