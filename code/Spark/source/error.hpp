#pragma once

typedef struct spark_error spark_error_t;

namespace Spark
{
    namespace Internal
    {
        extern char g_errorMessage[1024];
        extern void HandleException(spark_error_t** error, const std::exception& ex);

        template<typename FUNC>
        auto TranslateExceptions(spark_error_t** error, FUNC&& func) noexcept
        {
            try
            {
                return func();
            }
            catch(const std::exception& ex)
            {
                HandleException(error, ex);
            }
            return decltype(func())();
        }

        template<typename FUNC>
        auto TranslateExceptions(FUNC&& func) noexcept
        {
            return TranslateExceptions(nullptr, func);
        }
    }
}