#include "spark.hpp"

#include "error.hpp"

void spark_free_error(spark_error_t error)
{
    delete error;
}

const char* spark_get_error_message(spark_error_t error)
{
    return error->_message.c_str();
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

void spark_test_error(spark_error_t* error)
{
    Spark::Internal::TranslateExceptions(
        error,
        [&]()
        {
            throw std::runtime_error("Hello World Error");
        });
}
