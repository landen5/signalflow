add_executable(signalflow_tests test_signalflow.cpp)
target_link_libraries(signalflow_tests PRIVATE signalflow_lib GTest::gtest GTest::gtest_main)

add_executable(test_wav test_wav.cpp)
target_link_libraries(test_wav PRIVATE signalflow_lib GTest::gtest GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(signalflow_tests)
gtest_discover_tests(test_wav)