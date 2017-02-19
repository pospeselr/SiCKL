#pragma once

typedef struct spark_context spark_context_t;

extern "C" spark_context_t* spark_create_context(spark_error_t** error);
extern "C" void spark_set_current_context(spark_context_t* context, spark_error_t** error);
extern "C" void spark_destroy_context(spark_context_t* context, spark_error_t** error);