

# enable_testing()


# add_test(AllTestsInMain main)

# project("test_main")

include(GoogleTest)

option(ENABLE_PROFILER "enable dapr")

file(GLOB_RECURSE ALL_SOURCE *.cpp *.hxx)


include_directories(${CMAKE_SOURCE_DIR}/src/pc_pipe/include)
include_directories(${CMAKE_SOURCE_DIR}/src/pc_win/include)
include_directories(${CMAKE_SOURCE_DIR}/src/process_tools/include)


add_executable(${PROJECT_NAME} ${ALL_SOURCE})



gtest_discover_tests(${PROJECT_NAME}
                     WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})



target_compile_definitions(${PROJECT_NAME}
                           PRIVATE PROJECT_FOLDER="${CMAKE_SOURCE_DIR}")

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

find_package(GTest REQUIRED)

if((NOT WIN32) AND ENABLE_PROFILER)
  add_definitions(-DENABLE_PROFILER)
  target_link_libraries(
    ${PROJECT_NAME} profiler
    PRIVATE GTest::gmock
            GTest::gtest
            GTest::gmock_main
            GTest::gtest_main
            process_tools
)
else()
  target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE GTest::gmock
            GTest::gtest
            GTest::gmock_main
            GTest::gtest_main
            process_tools)
endif()
