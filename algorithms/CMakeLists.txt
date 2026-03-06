set(SOURCES src/Loda.cpp src/IForest.cpp)

add_library(algorithms ${SOURCES})

target_include_directories(algorithms PUBLIC include/)
target_link_libraries(algorithms PUBLIC utils compiler_flags sketches datasets)