cmake_minimum_required(VERSION 3.19)

option(VARI_TESTS_ENABLED "Enable tests" OFF)

project(vari)

add_library(
  vari
  INTERFACE
  include/vari/bits/typelist.h
  include/vari/bits/util.h
  include/vari/bits/ptr_core.h
  include/vari/bits/dispatch.h
  include/vari/uvptr.h
  include/vari/vptr.h
  include/vari/vref.h)
target_include_directories(vari INTERFACE include)
target_compile_features(vari INTERFACE cxx_std_20)

if(VARI_TESTS_ENABLED)
  include(CTest)
  if(BUILD_TESTING)

    add_executable(vptr_test test/vptr_test.cpp)
    target_link_libraries(vptr_test PUBLIC vari)
    target_include_directories(vptr_test PUBLIC deps/doctest/)
    add_test(NAME vptr_test COMMAND vptr_test)

    add_executable(example example.cpp)
    target_link_libraries(example PUBLIC vari)
    target_include_directories(example PUBLIC deps/doctest/)
    add_dependencies(example vptr_test)
    add_test(NAME example COMMAND example)

  endif()
endif()
