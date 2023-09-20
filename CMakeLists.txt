cmake_minimum_required(VERSION 3.20)

project(MandelbrotSet)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

##### External dependencies #####

if(UNIX)
    find_package(X11)
endif()

include(FetchContent)

FETCHCONTENT_DECLARE(
        GLFW
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG        7482de6071d21db77a7236155da44c172a7f6c9e  # version 3.3.8
)

FETCHCONTENT_DECLARE(
        GLEW
        GIT_REPOSITORY https://github.com/Perlmint/glew-cmake
        GIT_TAG        glew-cmake-2.2.0  # version 2.2.0
)

FETCHCONTENT_MAKEAVAILABLE(GLFW GLEW)

##### Include commands #####

add_executable(${PROJECT_NAME} src/main.cpp src/stb_image.cpp)
target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE dep)
target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE ${GLEW_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} glfw libglew_static)

##### Install commands #####

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
install(DIRECTORY img DESTINATION bin)
install(DIRECTORY shaders DESTINATION bin)
# TODO: exclude other files from being installed