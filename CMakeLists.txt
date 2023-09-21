cmake_minimum_required(VERSION 3.20)

project(MandelbrotSet)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

##### External dependencies #####

include(FetchContent)
set(FETCHCONTENT_QUIET OFF)

FETCHCONTENT_DECLARE(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG        7482de6071d21db77a7236155da44c172a7f6c9e  # version 3.3.8
)

FetchContent_GetProperties(glfw)
if(NOT glfw_POPULATED)
    FetchContent_Populate(glfw)
    add_subdirectory(${glfw_SOURCE_DIR} ${glfw_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

FETCHCONTENT_DECLARE(
        glew
        GIT_REPOSITORY https://github.com/Perlmint/glew-cmake
        GIT_TAG        glew-cmake-2.2.0  # version 2.2.0
)

FetchContent_GetProperties(glew)
if(NOT glew_POPULATED)
    FetchContent_Populate(glew)
    add_subdirectory(${glew_SOURCE_DIR} ${glew_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

##### Include commands #####

add_executable(${PROJECT_NAME} src/main.cpp src/stb_image.cpp)
target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE dep)
target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE ${glew_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} glfw libglew_static)

##### Install commands #####

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
install(DIRECTORY img DESTINATION bin)
install(DIRECTORY shaders DESTINATION bin)
