cmake_minimum_required(VERSION 3.16)

project(SA
    VERSION 1.0.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# ============================================================
# Hilos (Threads)
# ============================================================
find_package(Threads REQUIRED)

# ============================================================
# PostgreSQL / libpq
# ============================================================
find_package(PkgConfig REQUIRED)
pkg_check_modules(PostgreSQL REQUIRED libpq)

if(NOT PostgreSQL_FOUND)
    message(FATAL_ERROR "PostgreSQL/libpq not found. Install 'libpq-dev'.")
endif()

# ============================================================
# FetchContent (Dependencias externas automáticas)
# ============================================================
include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 10.1.1 
)

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.2
)

set(gtest_disable_pthreads ON CACHE BOOL "Disable pthreads" FORCE)
FetchContent_MakeAvailable(googletest fmt nlohmann_json)

# ============================================================
# Sources
# ============================================================
file(GLOB_RECURSE PROJECT_SOURCES "src/*.cpp" "src/*.cxx" "src/*.cc")
file(GLOB_RECURSE PROJECT_HEADERS "src/*.hpp" "src/*.h")
file(GLOB_RECURSE PROJECT_TESTS  "test/*.cpp")

# ============================================================
# Executable principal
# ============================================================
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${PROJECT_HEADERS})

target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Debug>:-O0 -g3 -Wall -Wextra -Wpedantic>
    $<$<CONFIG:Release>:-O3 -DNDEBUG>
    $<$<CONFIG:RelWithDebInfo>:-O2 -g>
)

target_include_directories(${PROJECT_NAME} PRIVATE 
    src
    ${PostgreSQL_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    ${PostgreSQL_LIBRARIES}
    fmt::fmt
    Threads::Threads
    nlohmann_json::nlohmann_json
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
)

# ============================================================
# Tests
# ============================================================
add_executable(${PROJECT_NAME}_tests ${PROJECT_TESTS})

target_link_libraries(${PROJECT_NAME}_tests PRIVATE
    gtest
    gtest_main
    fmt::fmt
    ${PostgreSQL_LIBRARIES}
    Threads::Threads
    nlohmann_json::nlohmann_json
)

target_include_directories(${PROJECT_NAME}_tests PRIVATE src ${PostgreSQL_INCLUDE_DIRS})

enable_testing()
add_test(NAME run_tests COMMAND ${PROJECT_NAME}_tests)

# ============================================================
# Custom Targets (GDB)
# ============================================================
add_custom_target(debug
    COMMAND gdb -ex run ${PROJECT_NAME}
    DEPENDS ${PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Launching GDB debugger..."
)