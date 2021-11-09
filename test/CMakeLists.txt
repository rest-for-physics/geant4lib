set(TESTING_EXECUTABLE RestGeant4Test)

file(GLOB TEST_SOURCES "src/*.cxx")

add_executable(
        ${TESTING_EXECUTABLE}
        ${TEST_SOURCES}
)

target_link_libraries(
        ${TESTING_EXECUTABLE}
        gtest_main
        RestGeant4 # TODO: This might only work if REST has been already installed, how can we do like in the packages to make it work?
)

include(GoogleTest)

gtest_discover_tests(${TESTING_EXECUTABLE})

file(COPY files/generators
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR})