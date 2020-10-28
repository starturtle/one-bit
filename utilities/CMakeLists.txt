add_library( utilities  
  Property.hpp
  logging.h
  error_codes.h
  setting_enums.h
  ArgumentParser.h
  logging.cpp
  ArgumentParser.cpp
)
target_include_directories( utilities PUBLIC "${DOCTEST_INCLUDE_DIR}" )
target_compile_definitions( utilities PRIVATE -DDOCTEST_CONFIG_DISABLE)

add_executable( argparser_test ArgumentParser.cpp )
target_include_directories( argparser_test PUBLIC ${CMAKE_SOURCE_DIR}/utilities )
target_link_libraries( argparser_test PUBLIC utilities )
target_compile_definitions( argparser_test PRIVATE -DDOCTEST_CONFIG_IMPLEMENT_WITH_MAIN )