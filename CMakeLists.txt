cmake_minimum_required(VERSION 3.10.2)

if (("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang") OR ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"))

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread")
  # [Optional] Use optimization options like -O1, -O2 or -O3 here:
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O1")

endif()

add_subdirectory(Engine)
add_subdirectory(Tests)
