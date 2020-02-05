# CMakeList.txt: toy_tracer 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
# SET(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
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
  ${CMAKE_CURRENT_SOURCE_DIR}/ext/glad/include
  ${CMAKE_CURRENT_SOURCE_DIR}/ext/std_image/include
  ${CMAKE_BINARY_DIR}/toy_tracer/ext/glog
)
# rapidjson, and include explicitly
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ext/rapidjson-1.1.0)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ext/rapidjson-1.1.0/include)
# image IO
file(GLOB IMAGE_IO ext/std_image/include/*.h
  ext/std_image/src/*.cpp
)
# glad
file(GLOB GLAD ${CMAKE_CURRENT_SOURCE_DIR}/ext/glad/src/*.c)

# assimp
INCLUDE_DIRECTORIES (
  ${CMAKE_CURRENT_SOURCE_DIR}/ext/assimp/include
)
# Don't process generated source files with AUTOMOC
if(POLICY CMP0071)
      cmake_policy(SET CMP0071 OLD)
endif(POLICY CMP0071)
# qt
set(CMAKE_AUTOMOC TRUE)
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

add_library(tracer_lib ${TRACER_LIB} ${IMAGE_IO} ${GLAD})
target_link_libraries(tracer_lib glog Qt5::Widgets)
if (MSVC)
  target_link_libraries(tracer_lib ${CMAKE_CURRENT_SOURCE_DIR}/ext/assimp/lib_x64/assimp.lib)
endif (MSVC)
# TODO: add link statements for Linux

file(GLOB TRACER_EXE
    main/canvas.cpp
    main/canvas.h
    main/main.cpp
    main/MainWindow.cpp
    main/MainWindow.h
    main/meshgen.cpp
    main/renderworker.cpp
    main/renderworker.h
    main/ResourceManager.cpp
    main/ResourceManager.h
    main/scene_object.cpp
    main/scne_object.h
    main/TwoThreadWQueue.h
    main/uiwrapper.cpp
    main/uiwrapper.h
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR})

message(STATUS "CMAKE CURRENT BINARY DIR: " ${CMAKE_CURRENT_BINARY_DIR})

add_executable (toytracer ${TRACER_EXE} ${ui_headers})
target_link_libraries(toytracer tracer_lib)

# copy shader files to the binary directory
add_custom_command(TARGET toytracer POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_CURRENT_SOURCE_DIR}/shader ${PROJECT_BINARY_DIR}/toy_tracer/shader)
if(MSVC)
  target_compile_options(toytracer PRIVATE "/MP")
  add_custom_command(TARGET toytracer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
      ${CMAKE_CURRENT_SOURCE_DIR}/ext/assimp/lib_x64/assimp-vc140-mt.dll ${PROJECT_BINARY_DIR}/toy_tracer)
endif (MSVC)

file(GLOB BALL_SRC
  main/canvas.cpp
  main/canvas.h
  main/ball.cpp
  main/renderworker.cpp
  main/renderworker.h
  main/TwoTheadQueue.h
  )

add_executable (ball ${BALL_SRC} ${ui_headers})
target_link_libraries(ball tracer_lib)

# copy shader files to the binary directory
add_custom_command(TARGET ball POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_CURRENT_SOURCE_DIR}/shader ${PROJECT_BINARY_DIR}/toy_tracer/shader)
if(MSVC)
  target_compile_options(ball PRIVATE "/MP")
  add_custom_command(TARGET ball POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
      ${CMAKE_CURRENT_SOURCE_DIR}/ext/assimp/lib_x64/assimp-vc140-mt.dll ${PROJECT_BINARY_DIR}/toy_tracer)
endif (MSVC)

# TODO: 如有需要，请添加测试并安装目标。
ENABLE_TESTING()

file(GLOB TRACER_TEST
    tests/test_interaction_transform.cpp
)

add_executable(tracer_test ${TRACER_TEST})
target_link_libraries(tracer_test tracer_lib)

add_test(tracer_unit_test tracer_test)