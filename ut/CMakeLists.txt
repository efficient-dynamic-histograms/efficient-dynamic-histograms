enable_testing()
add_executable(BenHaimTest BenHaimTest.cpp)

target_link_libraries(BenHaimTest PRIVATE GTest::gtest_main sketches compiler_flags)

add_executable(DADOTest DADOTest.cpp)

target_link_libraries(DADOTest PRIVATE GTest::gtest_main sketches compiler_flags)

add_executable(AllocatorTest AllocatorTest.cpp)

target_link_libraries(AllocatorTest PRIVATE GTest::gtest_main algorithms compiler_flags)

add_executable(DatasetTest DatasetTest.cpp)

target_link_libraries(DatasetTest PRIVATE GTest::gtest_main datasets compiler_flags)