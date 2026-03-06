cmake_minimum_required(VERSION 3.25)
project(MasterThesis CXX)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_library(compiler_flags INTERFACE)
target_compile_features(compiler_flags INTERFACE cxx_std_20)
set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU,LCC>")
target_compile_options(compiler_flags INTERFACE
  "$<${gcc_like_cxx}:-Wall;-Wextra;-Wshadow;-Wformat=2;-Wunused>"
  "$<$<CONFIG:Release>:-march=native;-funroll-loops;-fopenmp>"
)

cmake_policy(SET CMP0135 NEW)
include(FetchContent)
FetchContent_Declare(
  GTest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(GTest)


add_subdirectory(ut)
add_subdirectory(datasets)
add_subdirectory(topology)
add_subdirectory(utils)
add_subdirectory(sketches)
add_subdirectory(algorithms)
add_subdirectory(simulations)

add_executable(main main.cpp)
target_link_libraries(main PRIVATE compiler_flags datasets topology utils sketches )

add_executable(histograms histograms.cpp)
target_link_libraries(histograms PRIVATE compiler_flags datasets topology utils sketches )