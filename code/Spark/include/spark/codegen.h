#pragma once

extern "C" int32_t spark_node_to_text(Spark::Node* node, char* buffer, int32_t buffer_size, spark_error_t** error);
extern "C" int32_t spark_node_to_opencl(Spark::Node* node, char* buffer, int32_t buffer_size, spark_error_t** error);
