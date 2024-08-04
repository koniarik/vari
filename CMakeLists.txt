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

    add_executable(vval_test test/vval_test.cpp)
    target_link_libraries(vval_test PUBLIC vari)
    target_include_directories(vval_test PUBLIC deps/doctest/)
    add_test(NAME vval_test COMMAND vval_test)

    add_executable(example example.cpp)
    target_link_libraries(example PUBLIC vari)
    target_include_directories(example PUBLIC deps/doctest/)
    add_dependencies(example vptr_test)
    add_test(NAME example COMMAND example)

    add_executable(gdb_bin test/gdb_bin.cpp)
    target_link_libraries(gdb_bin PUBLIC vari)
    target_include_directories(gdb_bin PUBLIC deps/doctest/)

    set(GDB_SCRIPT ${CMAKE_BINARY_DIR}/gdb_bin.gdb)
    set(GDB_RESULT ${CMAKE_BINARY_DIR}/gdb.txt)
    add_test(NAME gdb_bin_g
             COMMAND gdb_bin gen ${CMAKE_CURRENT_SOURCE_DIR}/pprinter.py
                     ${GDB_SCRIPT})
    add_test(NAME gdb_bin_r COMMAND gdb gdb_bin -x ${GDB_SCRIPT} --batch)
    set_tests_properties(gdb_bin_r PROPERTIES FIXTURES_REQUIRED gdb_bin_g)
    add_test(NAME gdb_bin_e COMMAND gdb_bin eval ${GDB_RESULT})
    set_tests_properties(gdb_bin_e PROPERTIES FIXTURES_REQUIRED gdb_bin_r)

  endif()
endif()
