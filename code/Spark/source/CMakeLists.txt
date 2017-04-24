find_package(OpenCL 1.2 REQUIRED)

add_compile_options(-std=c++1z)
add_compile_options(-Wall)
add_compile_options(-Werror)
add_compile_options(-Wfatal-errors)
add_compile_options(-O3)
add_compile_options(-g)
add_compile_options(-fvisibility=hidden)

add_library(spark SHARED
    enums.cpp
    error.cpp
    node.cpp
    codegen.tree.cpp
    codegen.opencl.cpp
    runtime.cpp
    text_utilities.cpp)

target_link_libraries(spark OpenCL)

include_directories(../include)
include_directories(../../Ruff)
