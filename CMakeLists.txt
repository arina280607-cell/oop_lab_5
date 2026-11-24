cmake_minimum_required(VERSION 3.20)
project(Lab05 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

option(LAB05_SANITIZERS "Enable AddressSanitizer and UndefinedBehaviorSanitizer" OFF)

add_library(lab05_options INTERFACE)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(lab05_options INTERFACE -Wall -Wextra -Wpedantic)
    if(LAB05_SANITIZERS)
        target_compile_options(lab05_options INTERFACE
            -fsanitize=address,undefined -fno-omit-frame-pointer -g)
        target_link_options(lab05_options INTERFACE -fsanitize=address,undefined)
    endif()
elseif(LAB05_SANITIZERS)
    message(FATAL_ERROR "LAB05_SANITIZERS requires GCC or Clang")
endif()

add_library(lab05_core
    src/employee.cpp
    src/employee_tree.cpp
    src/selection.cpp
    src/criterion.cpp
)
target_include_directories(lab05_core PUBLIC src)
target_link_libraries(lab05_core PUBLIC lab05_options)

add_executable(lab_05 src/main.cpp)
target_link_libraries(lab_05 PRIVATE lab05_core)

include(CTest)
if(BUILD_TESTING)
    find_package(GTest CONFIG REQUIRED)
    add_executable(lab_05_tests tests/test_lab05.cpp)
    target_link_libraries(lab_05_tests PRIVATE lab05_core GTest::gtest_main)
    include(GoogleTest)
    # Source-based discovery also works with MinGW and Cyrillic paths.
    gtest_add_tests(TARGET lab_05_tests)
endif()
