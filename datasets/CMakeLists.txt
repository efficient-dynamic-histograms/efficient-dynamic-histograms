set(SOURCES src/Dataset.cpp src/FinalInstance.cpp src/Instance.cpp src/LabeledInstance.cpp src/Loader.cpp)

add_library(datasets ${SOURCES})

target_include_directories(datasets PUBLIC include/)
target_link_libraries(datasets PUBLIC utils compiler_flags)