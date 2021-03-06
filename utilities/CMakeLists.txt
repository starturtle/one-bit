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

if(DOCTEST_INCLUDE_DIR)
  message(STATUS "build utility tests")
  add_executable( test_argparser ArgumentParser.cpp )
  target_include_directories( test_argparser PUBLIC ${CMAKE_SOURCE_DIR}/utilities "${DOCTEST_INCLUDE_DIR}" )
  target_link_libraries( test_argparser PUBLIC utilities )
  target_compile_definitions( test_argparser PRIVATE -DDOCTEST_CONFIG_IMPLEMENT_WITH_MAIN )
  
  add_executable( test_calculus calculus.cpp )
  target_include_directories( test_calculus PUBLIC ${CMAKE_SOURCE_DIR}/utilities "${DOCTEST_INCLUDE_DIR}" )
  target_link_libraries( test_calculus PUBLIC utilities )
  target_compile_definitions( test_calculus PRIVATE -DDOCTEST_CONFIG_IMPLEMENT_WITH_MAIN )
  
  add_executable( test_logging logging.cpp )
  target_include_directories( test_logging PUBLIC ${CMAKE_SOURCE_DIR}/utilities "${DOCTEST_INCLUDE_DIR}" )
  target_link_libraries( test_logging PUBLIC utilities )
  target_compile_definitions( test_logging PRIVATE -DDOCTEST_CONFIG_IMPLEMENT_WITH_MAIN )
endif()