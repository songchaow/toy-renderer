# CMakeList.txt: toy_tracer 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
SET(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
cmake_minimum_required (VERSION 3.8)

# third-party dependencies
# glog

SET(WITH_GFLAGS OFF CACHE BOOL "Use gflags")
SET(BUILD_SHARED_LIBS OFF CACHE BOOL " " FORCE)
IF(WIN32)
  ADD_DEFINITIONS( -D GOOGLE_GLOG_DLL_DECL= )
ENDIF()
ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/ext/glog)
SET_PROPERTY(TARGET glog logging_unittest demangle_unittest utilities_unittest stl_logging_unittest PROPERTY FOLDER "ext")
INCLUDE_DIRECTORIES (
  ${CMAKE_CURRENT_SOURCE_DIR}/ext/glog/src
  ${CMAKE_BINARY_DIR}/toy_tracer/ext/glog
)
message(STATUS "Binary dir: " ${CMAKE_BINARY_DIR})

# image IO
file(GLOB IMAGE_IO ext/std_image/include/*.h
  ext/std_image/src/*.cpp
)

# assimp
INCLUDE_DIRECTORIES (
  ${CMAKE_CURRENT_SOURCE_DIR}/ext/assimp/include
)

# qt
set(CMAKE_AUTOMOC ON)
find_package(Qt5 COMPONENTS Widgets REQUIRED)

qt5_wrap_ui(ui_headers main/mainwindow.ui)

# toy tracer
file(GLOB TRACER_LIB 
*.cpp *.h *.hpp
core/*.cpp
light/*.cpp
utils/*.cpp
texture/*.cpp
shape/*.cpp
)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

add_library(tracer_lib ${TRACER_LIB} ${IMAGE_IO})
target_link_libraries(tracer_lib glog Qt5::Widgets)
if (MSVC)
  target_link_libraries(tracer_lib ${CMAKE_CURRENT_SOURCE_DIR}/ext/assimp/lib_x64/assimp.lib)
endif (MSVC)
# TODO: add link statements for Linux

file(GLOB TRACER_EXE
    main/*.cpp
    main/*.h
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

add_executable (toytracer ${TRACER_EXE} ${ui_headers})
target_link_libraries(toytracer tracer_lib)

# copy shader files to the binary directory
add_custom_command(TARGET toytracer POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_CURRENT_SOURCE_DIR}/shader $<TARGET_FILE_DIR:toytracer>)

# TODO: 如有需要，请添加测试并安装目标。
ENABLE_TESTING()

file(GLOB TRACER_TEST
    tests/test_interaction_transform.cpp
)

add_executable(tracer_test ${TRACER_TEST})
target_link_libraries(tracer_test tracer_lib)

add_test(tracer_unit_test tracer_test)