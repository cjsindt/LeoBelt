# License: Apache 2.0. See LICENSE file in root directory.
# Copyright(c) 2019 Intel Corporation. All Rights Reserved.
#  minimum required cmake version: 3.1.0
cmake_minimum_required(VERSION 3.1.0)

project(leo-belt)
#project(server)
project(leo-belt-headless)
project(shutdown)

# Save the command line compile commands in the build output
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# Make project require C++11
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
endif()



find_library(REALSENSE2_FOUND realsense2 HINTS ${LIBRARY_DIR} REQUIRED)
find_package(glfw3)
find_library(REALSENSE2GL_FOUND realsense2-gl HINTS ${LIBRARY_DIR} REQUIRED)
find_package(glut)
find_package(GL)
find_package(GLU)

find_package(SFML 2 COMPONENTS graphics window REQUIRED)

if(NOT REALSENSE2_FOUND)
    SET(REALSENSE2_FOUND "realsense2")
    message(WARN "Failed to find_library(realsense2)")
endif()

    configure_file(white-square.png white-square.png COPYONLY)

    add_executable(leo-belt leo-belt.cpp third-party/imgui/imgui.cpp third-party/imgui/imgui_draw.cpp third-party/imgui/imgui_impl_glfw.cpp rs232.c)
    #add_executable(server server.cpp)
    add_executable(leo-belt-headless leo-belt-headless.cpp rs232.c)
    add_executable(shutdown shutdown.cpp rs232.c)
    set_property(TARGET leo-belt PROPERTY CXX_STANDARD 11)
    set_property(TARGET leo-belt-headless PROPERTY CXX_STANDARD 11)
    set_property(TARGET shutdown PROPERTY CXX_STANDARD 11)
    target_link_libraries(leo-belt ${REALSENSE2_FOUND})
    #target_link_libraries(server ${REALSENSE2_FOUND})
    target_link_libraries(leo-belt-headless ${REALSENSE2_FOUND})
    target_link_libraries(leo-belt ${REALSENSE2GL_FOUND})
    #target_link_libraries(server ${REALSENSE2GL_FOUND})
    target_link_libraries(leo-belt-headless ${REALSENSE2GL_FOUND})
    target_link_libraries(leo-belt ${DEPENDENCIES})
    target_link_libraries(leo-belt glfw)
    target_link_libraries(leo-belt pthread)
    target_link_libraries(leo-belt glut)
    target_link_libraries(leo-belt GL)
    target_link_libraries(leo-belt GLU)
    target_link_libraries(leo-belt-headless ${DEPENDENCIES})
    target_link_libraries(leo-belt-headless glfw)
    target_link_libraries(leo-belt-headless pthread)
    target_link_libraries(leo-belt-headless glut)
    target_link_libraries(leo-belt-headless GL)
    target_link_libraries(leo-belt-headless GLU)
    target_link_libraries(leo-belt sfml-graphics)
    target_link_libraries(leo-belt sfml-window)
  
   
    include_directories(common third-party/imgui)

    set_target_properties (leo-belt PROPERTIES FOLDER Examples)


install(
    TARGETS

    leo-belt

    CONFIGURATIONS Release
    RUNTIME DESTINATION
    ${CMAKE_INSTALL_PREFIX}/bin
)
install(
    TARGETS

    leo-belt-headless

    CONFIGURATIONS Release
    RUNTIME DESTINATION
    ${CMAKE_INSTALL_PREFIX}/bin
)
install(
    TARGETS

    shutdown

    CONFIGURATIONS Release
    RUNTIME DESTINATION
    ${CMAKE_INSTALL_PREFIX}/bin
)
