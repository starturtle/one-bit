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
target_include_directories( imaging PUBLIC ${OpenCV_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/utilities "${DOCTEST_INCLUDE_DIR}" )
target_link_libraries( imaging PUBLIC ${OpenCV_LIBS} pixelator)
target_compile_definitions(imaging PRIVATE -DDOCTEST_CONFIG_DISABLE)

add_executable( pixelator_test Pixelator.cpp )
target_include_directories( pixelator_test PUBLIC ${OpenCV_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/utilities "${DOCTEST_INCLUDE_DIR}" )
target_link_libraries( pixelator_test PUBLIC ${OpenCV_LIBS} )
target_compile_definitions( pixelator_test PRIVATE -DDOCTEST_CONFIG_IMPLEMENT_WITH_MAIN )

add_executable( shrink_pixelator_test ShrinkPixelator.cpp )
target_include_directories( shrink_pixelator_test PUBLIC ${OpenCV_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/utilities "${DOCTEST_INCLUDE_DIR}" )
target_link_libraries( shrink_pixelator_test PUBLIC ${OpenCV_LIBS} pixelator)
target_compile_definitions( shrink_pixelator_test PRIVATE -DDOCTEST_CONFIG_IMPLEMENT_WITH_MAIN -DIGNORE_BASE_CLASS_TESTS)