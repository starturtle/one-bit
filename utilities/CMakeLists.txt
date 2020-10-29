add_library( utilities  
  Property.hpp
  logging.h
  error_codes.h
  setting_enums.h
  ArgumentParser.h
  logging.cpp
  ArgumentParser.cpp
  calculus.h
  calculus.cpp
)

if(${BUILD_TESTING})
  add_executable( test_argparser ArgumentParser.cpp )
  target_include_directories( test_argparser PUBLIC ${CMAKE_SOURCE_DIR}/utilities "${DOCTEST_INCLUDE_DIR}" )
  target_link_libraries( test_argparser PUBLIC utilities )
  target_compile_definitions( test_argparser PRIVATE -DDOCTEST_CONFIG_IMPLEMENT_WITH_MAIN )
  
  add_executable( test_calculus calculus.cpp )
  target_include_directories( test_calculus PUBLIC ${CMAKE_SOURCE_DIR}/utilities "${DOCTEST_INCLUDE_DIR}" )
  target_link_libraries( test_calculus PUBLIC utilities )
  target_compile_definitions( test_calculus PRIVATE -DDOCTEST_CONFIG_IMPLEMENT_WITH_MAIN )
endif()