find_package( OpenCV REQUIRED )
add_library( imaging  
  Pixelator.cpp
  ShrinkPixelator.cpp
  Pixelator.h
  ShrinkPixelator.h
)
target_include_directories( imaging PUBLIC ${OpenCV_INCLUDE_DIRS} ../utilities "${DOCTEST_INCLUDE_DIR}" )
target_link_libraries( imaging PUBLIC ${OpenCV_LIBS} )