add_compile_options(-std=c++1z)
add_compile_options(-Wall)
add_compile_options(-Werror)
add_compile_options(-Wfatal-errors)
add_compile_options(-O0)
add_compile_options(-g)
add_compile_options(-fvisibility=hidden)

add_library(thistle SHARED
    thistle.cpp
    thistle_error.cpp
    thistle_buffer.cpp
    thistle_node.cpp
    thistle_linear_transform_node.cpp
    thistle_label_node.cpp
    thistle_parameter_updater.cpp
    thistle_sgd_parameter_updater.cpp)

target_link_libraries(thistle spark)

include_directories(../../Ruff)
include_directories(../../Spark/include)
include_directories(../include)