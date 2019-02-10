# CMakeList.txt: toy_tracer 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

# 将源代码添加到此项目的可执行文件。
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
file(GLOB TRACER_TEST
    tests/*.cpp
)
add_test(tracer_unit_test ${TRACER_TEST} ${TRACER_LIB})