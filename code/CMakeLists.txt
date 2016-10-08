cmake_minimum_required(VERSION 3.5.1)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/")
message("CMAKE_MODULE_PATH: " ${CMAKE_MODULE_PATH})

include (PrecompiledHeader)

project(SPARK_ROOT)

add_subdirectory(Spark)
add_subdirectory(SparkTest)