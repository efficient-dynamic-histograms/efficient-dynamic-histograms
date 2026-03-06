set(SOURCES src/BenHaim.cpp src/DADO.cpp src/KDEBenHaim.cpp)

add_library(sketches ${SOURCES})

target_include_directories(sketches PUBLIC include/)
target_link_libraries(sketches PUBLIC utils compiler_flags)