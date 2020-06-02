find_package( OpenCV REQUIRED )
add_library( imaging  
    Pixelator.cpp
    FloodFillPixelator.cpp
    ShrinkPixelator.cpp
    Pixelator.h
    FloodFillPixelator.h
    ShrinkPixelator.h
)
target_include_directories( imaging PUBLIC ${OpenCV_INCLUDE_DIRS} )
target_link_libraries( imaging PUBLIC ${OpenCV_LIBS} )