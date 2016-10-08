add_compile_options(-std=gnu++14)

add_library(spark SHARED
    enums.cpp
    error.cpp
    node.cpp)

include_directories(../include)
