add_compile_options(-std=c++1z)
add_compile_options(-Wfatal-errors)
add_compile_options(-O3)
add_compile_options(-g)

add_library(spark SHARED
    enums.cpp
    error.cpp
    node.cpp
    node.text.cpp)

include_directories(../include)
