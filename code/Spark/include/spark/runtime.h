#pragma once

typedef struct spark_context spark_context_t;
typedef struct spark_kernel spark_kernel_t;
typedef struct spark_buffer spark_buffer_t;

extern "C" spark_context_t* spark_create_context(spark_error_t** error);
extern "C" void spark_set_current_context(spark_context_t* context, spark_error_t** error);
extern "C" void spark_destroy_context(spark_context_t* context, spark_error_t** error);

extern "C" spark_kernel_t* spark_create_kernel(spark_node_t* root, spark_error_t** error);
extern "C" const char* spark_get_kernel_source(spark_kernel_t* kernel, spark_error_t** error);
extern "C" void spark_destroy_kernel(spark_kernel_t* kernel, spark_error_t** error);

extern "C" spark_buffer_t* spark_create_buffer(size_t bytes, const void* data, spark_error_t** error);
extern "C" void spark_write_buffer(spark_buffer_t* buffer, size_t offset, size_t bytes, const void* data, spark_error_t** error);
extern "C" void spark_read_buffer(spark_buffer_t* buffer, size_t offset, size_t bytes, void* dest, spark_error_t** error);
extern "C" void spark_destroy_buffer(spark_buffer_t* buffer, spark_error_t** error);