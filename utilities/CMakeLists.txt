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
target_compile_definitions(utilities PRIVATE -DDOCTEST_CONFIG_DISABLE)

add_library( utilities_test  
  Property.hpp
  logging.h
  error_codes.h
  setting_enums.h
  ArgumentParser.h
  logging.cpp
  ArgumentParser.cpp
)
target_include_directories( utilities_test PUBLIC "${DOCTEST_INCLUDE_DIR}" )
target_compile_definitions(utilities_test PRIVATE -DDOCTEST_CONFIG_IMPLEMENT)