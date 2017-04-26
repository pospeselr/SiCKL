add_compile_options(-std=gnu++1z)
add_compile_options(-Wfatal-errors)
add_compile_options(-O3)
add_compile_options(-g)

add_executable(thistle_test
    main.cpp)

target_link_libraries(thistle_test spark)
target_link_libraries(thistle_test thistle)

include_directories(../../Ruff)
include_directories(../../Spark/include)
include_directories(../../Thistle/include)