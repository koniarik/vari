# Builds against an installed vari; point CMAKE_PREFIX_PATH at the install
# prefix.
cmake_minimum_required(VERSION 3.19)

project(vari_install_consumer CXX)

find_package(vari REQUIRED)

add_executable(consumer main.cpp)
target_link_libraries(consumer vari::vari)
