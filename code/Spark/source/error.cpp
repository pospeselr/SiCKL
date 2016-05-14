#include "precomp.h"

#include "internal/common.h"
#include "internal/error.h"

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