# FIND_PACKAGE_ARGS of FetchContent_Declare needs CMake 3.24.
cmake_minimum_required(VERSION 3.24)

file(GLOB TESTS ./*_test.cpp)

include(FetchContent)
FetchContent_Declare(
  doctest
  GIT_REPOSITORY https://github.com/doctest/doctest.git
  GIT_TAG v2.4.12
  FIND_PACKAGE_ARGS 2.4.12)
set(DOCTEST_NO_INSTALL
    ON
    CACHE BOOL "")
FetchContent_MakeAvailable(doctest)

add_executable(vari_utest ${TESTS})
target_link_libraries(vari_utest PUBLIC vari doctest::doctest)
add_test(NAME vari_utest COMMAND vari_utest)

add_executable(example ../example.cpp)
target_link_libraries(example PUBLIC vari doctest::doctest)
add_test(NAME example COMMAND example)

add_executable(gdb_bin ./gdb_bin.cpp)
target_link_libraries(gdb_bin PUBLIC vari doctest::doctest)

find_program(VARI_GDB gdb)
if(VARI_GDB
   AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU"
   AND CMAKE_SYSTEM_NAME STREQUAL "Linux")
  set(GDB_SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/gdb_bin.gdb)
  set(GDB_RESULT ${CMAKE_CURRENT_BINARY_DIR}/gdb.txt)
  add_test(NAME gdb_bin_g
           COMMAND gdb_bin gen ${CMAKE_CURRENT_SOURCE_DIR}/../pprinter.py
                   ${GDB_SCRIPT})
  set_tests_properties(gdb_bin_g PROPERTIES FIXTURES_SETUP gdb_script)
  add_test(NAME gdb_bin_r COMMAND ${VARI_GDB} $<TARGET_FILE:gdb_bin> -x
                                  ${GDB_SCRIPT} --batch)
  set_tests_properties(gdb_bin_r PROPERTIES FIXTURES_REQUIRED gdb_script
                                            FIXTURES_SETUP gdb_log)
  add_test(NAME gdb_bin_e COMMAND gdb_bin eval ${GDB_RESULT})
  set_tests_properties(gdb_bin_e PROPERTIES FIXTURES_REQUIRED gdb_log)
endif()

find_package(
  Python3
  COMPONENTS Interpreter
  REQUIRED)
add_custom_command(
  OUTPUT ${CMAKE_BINARY_DIR}/readme.cpp
  COMMAND
    ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/gen_readme_cpp.py
    ${CMAKE_CURRENT_SOURCE_DIR}/../README.md ${CMAKE_BINARY_DIR}/readme.cpp
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../README.md
          ${CMAKE_CURRENT_SOURCE_DIR}/gen_readme_cpp.py)
add_executable(readme ${CMAKE_BINARY_DIR}/readme.cpp)
target_link_libraries(readme PUBLIC vari doctest::doctest)
add_test(NAME readme COMMAND readme)
