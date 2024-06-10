
cmake_minimum_required(VERSION 3.19)
project(vari)

add_library(vari INTERFACE
	include/vari/typelist.h
	include/vari/uvptr.h
	include/vari/vptr.h
	include/vari/vref.h
	)
target_include_directories(vari INTERFACE include)
target_compile_features(vari INTERFACE cxx_std_20)

include(CTest)

add_executable(vptr_test test/vptr_test.cpp)
target_link_libraries(vptr_test PUBLIC vari)
target_include_directories(vptr_test PUBLIC deps/doctest/)
add_test(NAME vptr_test COMMAND vptr_test)