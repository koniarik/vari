cmake_minimum_required(VERSION 3.19)

option(VARI_TESTS_ENABLED "Enable tests" OFF)

project(vari)

file(GLOB VARI_HEADERS include/vari/*.h include/vari/bits/*.h)

include(CMakePackageConfigHelpers)

add_library(vari INTERFACE ${VARI_HEADERS})
target_include_directories(
  vari INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                 $<INSTALL_INTERFACE:include>)
target_compile_features(vari INTERFACE cxx_std_20)

add_library(vari::vari ALIAS vari)

install(DIRECTORY include/vari/ DESTINATION include/vari/)

install(
  TARGETS vari
  EXPORT vari
  INCLUDES
  DESTINATION include/vari/)
install(
  EXPORT vari
  FILE vari-config.cmake
  NAMESPACE vari::
  DESTINATION lib/cmake/vari/)

if(VARI_TESTS_ENABLED)
  include(CTest)
  if(BUILD_TESTING)

    file(GLOB TESTS test/*_test.cpp)

    add_library(doctest INTERFACE)
    target_include_directories(doctest INTERFACE deps/)

    add_executable(vari_utest ${TESTS})
    target_link_libraries(vari_utest PUBLIC vari doctest)
    add_test(NAME vari_utest COMMAND vari_utest)

    add_executable(example example.cpp)
    target_link_libraries(example PUBLIC vari doctest)
    add_test(NAME example COMMAND example)

    add_executable(gdb_bin test/gdb_bin.cpp)
    target_link_libraries(gdb_bin PUBLIC vari doctest)

    set(GDB_SCRIPT ${CMAKE_BINARY_DIR}/gdb_bin.gdb)
    set(GDB_RESULT ${CMAKE_BINARY_DIR}/gdb.txt)
    add_test(NAME gdb_bin_g
             COMMAND gdb_bin gen ${CMAKE_CURRENT_SOURCE_DIR}/pprinter.py
                     ${GDB_SCRIPT})
    add_test(NAME gdb_bin_r COMMAND gdb gdb_bin -x ${GDB_SCRIPT} --batch)
    set_tests_properties(gdb_bin_r PROPERTIES FIXTURES_REQUIRED gdb_bin_g)
    add_test(NAME gdb_bin_e COMMAND gdb_bin eval ${GDB_RESULT})
    set_tests_properties(gdb_bin_e PROPERTIES FIXTURES_REQUIRED gdb_bin_r)

    add_custom_command(
      OUTPUT ${CMAKE_BINARY_DIR}/readme.cpp
      COMMAND
        python3 ${CMAKE_CURRENT_SOURCE_DIR}/test/gen_readme_cpp.py
        ${CMAKE_CURRENT_SOURCE_DIR}/README.md ${CMAKE_BINARY_DIR}/readme.cpp
      DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/README.md
              ${CMAKE_CURRENT_SOURCE_DIR}/test/gen_readme_cpp.py)
    add_executable(readme ${CMAKE_BINARY_DIR}/readme.cpp)
    target_link_libraries(readme PUBLIC vari doctest)
    add_test(NAME readme COMMAND readme)

  endif()
endif()
