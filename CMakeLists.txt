cmake_minimum_required(VERSION 2.8)

project(hybrid-cpp-lru-cache)

set (CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)
find_package(GTest MODULE REQUIRED)

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

enable_testing()

include_directories(
    ${GTEST_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/include)

add_executable(${PROJECT_NAME} src/test)

add_dependencies(${PROJECT_NAME} googletest)

target_link_libraries(
    ${PROJECT_NAME}
    GTest::GTest
    GTest::Main
    ${CMAKE_THREAD_LIBS_INIT})

set_target_properties(${PROJECT_NAME} PROPERTIES
    PREFIX ""
    SUFFIX ""
    COMPILE_FLAGS "-std=c++0x -W -Wall -pedantic")

set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3")
set(CMAKE_CSS_FLAGS_RELEASE "-03 -g")

add_test(
    NAME ${PROJECT_NAME}-test
    COMMAND ${PROJECT_NAME}-test
)

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --verbose
    DEPENDS ${PROJECT_NAME}-test)
