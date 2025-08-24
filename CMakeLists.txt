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
  enable_testing()
  add_subdirectory(test)
endif()
