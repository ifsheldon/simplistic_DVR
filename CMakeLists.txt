cmake_minimum_required(VERSION 3.17)
project(simplistic_DVR)

set(CMAKE_CXX_STANDARD 14)
find_package(glfw3 3.2.1 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(X11 REQUIRED)
link_libraries(${X11_LIBRARIES})
include_directories(${X11_INCLUDE_DIR})
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

include_directories("${CMAKE_SOURCE_DIR}/lib/glad/include/")
include_directories("${CMAKE_SOURCE_DIR}/src/include/")
include_directories("${CMAKE_SOURCE_DIR}/lib/")
include_directories("/usr/include/")
include_directories("${CMAKE_SOURCE_DIR}/lib/NumCpp/include")


add_executable(simplistic_DVR src/main.cpp
        src/glslprogram.cpp
        ${CMAKE_SOURCE_DIR}/lib/glad/src/glad.c
        src/include/glslprogram.h
        src/include/tf_tables.h
        src/include/utils.h
        src/include/consts.h
        src/include/shading.h src/include/vbocube.h src/include/vborectangle.h)

target_link_libraries(simplistic_DVR PUBLIC glfw OpenGL::GL ${CMAKE_DL_LIBS})
