add_executable(signalflow_tests test_signalflow.cpp)
target_link_libraries(signalflow_tests PRIVATE signalflow_lib GTest::gtest GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(signalflow_tests)
