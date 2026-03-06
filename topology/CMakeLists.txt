set(SOURCES src/Metrics.cpp)

add_library(topology ${SOURCES})

target_include_directories(topology PUBLIC include/)
target_link_libraries(topology PUBLIC datasets compiler_flags)