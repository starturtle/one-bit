find_package( OpenCV REQUIRED )
add_library( imaging  
  Pixelator.cpp
  ShrinkPixelator.cpp
  Pixelator.h
  ShrinkPixelator.h
)
target_include_directories( imaging PUBLIC ${OpenCV_INCLUDE_DIRS} ../utilities "${DOCTEST_INCLUDE_DIR}" )
target_link_libraries( imaging PUBLIC ${OpenCV_LIBS} )
target_compile_definitions(imaging PRIVATE -DDOCTEST_CONFIG_DISABLE)
add_library( imaging_test  
  Pixelator.cpp
  ShrinkPixelator.cpp
  Pixelator.h
  ShrinkPixelator.h
)
target_include_directories( imaging_test PUBLIC ${OpenCV_INCLUDE_DIRS} ../utilities "${DOCTEST_INCLUDE_DIR}" )
target_link_libraries( imaging_test PUBLIC ${OpenCV_LIBS} )
target_compile_definitions(imaging_test PRIVATE -DDOCTEST_CONFIG_IMPLEMENT)