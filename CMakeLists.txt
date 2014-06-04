cmake_minimum_required(VERSION 2.8.5)
project(shad0r C CXX)

if (${CMAKE_C_COMPILER_ID} MATCHES GNU)
  set(CMAKE_C_FLAGS "-std=c99 -Wall -Werror")
endif (${CMAKE_C_COMPILER_ID} MATCHES GNU)

# Don't strip RPATH when installing
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

add_subdirectory(src/mixer2)
