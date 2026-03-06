set(SOURCES src/Math.cpp)

add_library(utils ${SOURCES})

target_include_directories(utils PUBLIC include/)
target_link_libraries(utils PUBLIC compiler_flags)