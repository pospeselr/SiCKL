add_compile_options(-std=gnu++14)
add_compile_options(-Wfatal-errors)

add_library(spark SHARED
    enums.cpp
    error.cpp
    node.cpp
    node.text.cpp)

include_directories(../include)
