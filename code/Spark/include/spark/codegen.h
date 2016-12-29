#pragma once

extern "C" int32_t spark_node_to_text(spark_node_t* node, char* buffer, int32_t buffer_size, spark_error_t** error);
extern "C" int32_t spark_node_to_opencl(spark_node_t* node, char* buffer, int32_t buffer_size, spark_error_t** error);
