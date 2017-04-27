#include "thistle.hpp"

#include "thistle_error.hpp"

using ruff::translate_exceptions;

static spark_context_t* g_spark_context = nullptr;

RUFF_EXPORT void thistle_begin_session(thistle_error_t** error)
{
    return translate_exceptions(error, [&]()
    {
        RUFF_THROW_IF_FALSE(g_spark_context == nullptr);
        g_spark_context = spark_create_context(SPARK_THROW_ON_ERROR());
        spark_set_current_context(g_spark_context, SPARK_THROW_ON_ERROR());
    });
}

RUFF_EXPORT void thistle_end_session(thistle_error_t** error)
{
    return translate_exceptions(error, [&]()
    {
        RUFF_THROW_IF_NULL(g_spark_context);
        spark_destroy_context(g_spark_context, SPARK_THROW_ON_ERROR());
        g_spark_context = nullptr;
    });
}