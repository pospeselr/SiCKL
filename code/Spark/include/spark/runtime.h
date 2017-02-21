#pragma once

typedef struct spark_context spark_context_t;
typedef struct spark_kernel spark_kernel_t;

extern "C" spark_context_t* spark_create_context(spark_error_t** error);
extern "C" void spark_set_current_context(spark_context_t* context, spark_error_t** error);
extern "C" void spark_destroy_context(spark_context_t* context, spark_error_t** error);

extern "C" spark_kernel_t* spark_create_kernel(spark_node_t* root, spark_error_t** error);
extern "C" const char* spark_get_kernel_source(spark_kernel_t* kernel, spark_error_t** error);
extern "C" void spark_destroy_kernel(spark_kernel_t* kernel, spark_error_t** error);
