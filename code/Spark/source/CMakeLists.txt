add_compile_options(-std=gnu++14)

add_library(spark SHARED
    enums.cpp
    error.cpp
    node.cpp
    precomp.h
    precomp.cpp)

add_precompiled_header(spark
    precomp.h FORCEINCLUDE
    SOURCE_CXX precomp.cpp)

include_directories(../include)
