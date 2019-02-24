# CMakeList.txt: toy_tracer 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

# 将源代码添加到此项目的可执行文件。

# glog

SET(WITH_GFLAGS OFF CACHE BOOL "Use gflags")
SET(BUILD_SHARED_LIBS OFF CACHE BOOL " " FORCE)
IF(WIN32)
  ADD_DEFINITIONS( -D GOOGLE_GLOG_DLL_DECL= )
ENDIF()
ADD_SUBDIRECTORY(ext/glog)
SET_PROPERTY(TARGET glog logging_unittest demangle_unittest utilities_unittest stl_logging_unittest PROPERTY FOLDER "ext")
INCLUDE_DIRECTORIES (
  ext/glog/src
  ${CMAKE_BINARY_DIR}/toy_tracer/ext/glog
)

# toy tracer
file(GLOB TRACER_LIB 
    *.cpp *.h *.hpp
    core/*.cpp
    utils/*.cpp
    
)
file(GLOB TRACER_EXE
    main/*.cpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable (toytracer ${TRACER_LIB} ${TRACER_EXE})

# TODO: 如有需要，请添加测试并安装目标。
ENABLE_TESTING()

file(GLOB TRACER_TEST
    tests/*.cpp
)

add_executable(tracer_test ${TRACER_TEST} ${TRACER_LIB})

add_test(tracer_unit_test tracer_test)