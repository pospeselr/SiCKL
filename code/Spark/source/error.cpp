#include "spark.hpp"

#include "error.hpp"

struct spark_error
{
    spark_error(const char* msg) : _error_message(msg) {}
    const char* _error_message;
};

namespace Spark
{
    namespace Internal
    {
        char g_errorMessage[1024] = {};

        void HandleException(spark_error_t** error, const std::exception& ex)
        {
            if(error)
            {
                snprintf(g_errorMessage, countof(g_errorMessage), "%s", ex.what());
                *error = new spark_error(g_errorMessage);
            }
        }
    }
}

// error handling
void spark_print_assert(const char* msg, const char* file, uint32_t line)
{
	fprintf(stderr, "Spark assertion failure %s:%u \"%s\"\n", file, line, msg);
	DEBUGBREAK();
}

void spark_print_exception(const std::exception& ex)
{
    const auto msg = ex.what();
    fprintf(stderr, "Spark caught C++ exception:\n%s\n", msg);
    DEBUGBREAK();
}

void spark_free_error(spark_error_t* error)
{
    delete error;
}

const char* spark_get_error_message(spark_error_t* error)
{
    return error->_error_message;
}

void spark_test_error(spark_error_t** error)
{
    return Spark::Internal::TranslateExceptions(
        error,
        [&]()
        {
            throw std::runtime_error("Hello World Error");
        });
}

