#pragma once

typedef struct spark_error spark_error_t;

extern "C" void spark_destroy_error(spark_error_t* error);
extern "C" const char* spark_get_error_message(spark_error_t* error);

namespace spark
{
    namespace client
    {
        using throw_on_error = ruff::throw_on_error<
            spark_error_t,
            decltype(&::spark_destroy_error),
            &::spark_destroy_error,
            decltype(&::spark_get_error_message),
            &::spark_get_error_message>;
    }
}

#define SPARK_THROW_ON_ERROR() spark::client::throw_on_error()
