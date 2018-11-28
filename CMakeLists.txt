if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread")

elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU") # GCC

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread")

endif()

add_subdirectory(Engine)
add_subdirectory(QuickTests)
