add_compile_options(-std=gnu++14 -O3)

add_executable(spark_test
    main.cpp)

target_link_libraries(spark_test spark)

include_directories(../../Spark/include)