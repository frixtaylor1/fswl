cmake_minimum_required(VERSION 3.16)

project(SA
    VERSION 1.0.0
    LANGUAGES CXX
)

# ============================================================
#   Global Settings
# ============================================================
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# ============================================================
#   Sources
# ============================================================
file(GLOB_RECURSE PROJECT_SOURCES "src/*.cpp" "src/*.cxx" "src/*.cc")
file(GLOB_RECURSE PROJECT_HEADERS "src/*.hpp" "src/*.h")

# ============================================================
#   Main executable
# ============================================================
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${PROJECT_HEADERS})

# ============================================================
#   Compiler flags (modern style)
# ============================================================
target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Debug>:-O0 -g3 -Wall -Wextra -Wpedantic>
    $<$<CONFIG:Release>:-O3 -DNDEBUG>
    $<$<CONFIG:RelWithDebInfo>:-O2 -g>
)

# ============================================================
#   Include directories
# ============================================================
target_include_directories(${PROJECT_NAME} PRIVATE src)

# ============================================================
#   Link pthread explicitly (ThreadPool)
# ============================================================
target_link_libraries(${PROJECT_NAME} PRIVATE pthread)

# ============================================================
#   Preprocessor definitions
# ============================================================
target_compile_definitions(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
)

# ============================================================
#   Debug target (launch gdb)
# ============================================================
add_custom_target(debug
    COMMAND gdb -ex run ${PROJECT_NAME}
    DEPENDS ${PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Launching GDB debugger..."
)

# ============================================================
#   Folder structure (for IDEs)
# ============================================================
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source Files" FILES ${PROJECT_SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Header Files" FILES ${PROJECT_HEADERS})
