#pragma once

#ifdef SPARK_DEBUG
	#define SPARK_ASSERT(X) do { if(!(X)) { spark::client::print_assert(#X, __FILE__, (uint32_t)__LINE__); DEBUGBREAK(); } } while(0)
	#define SPARK_VERIFY(X) SPARK_ASSERT(X)
#else
	#define SPARK_ASSERT(X) (void)0
	#define SPARK_VERIFY(X) (X)
#endif

#define SPARK_STATIC_ASSERT(...) static_assert((__VA_ARGS__), #__VA_ARGS__)

#define THROW_ON_ERROR() spark::client::ThrowOnError()

typedef struct spark_error spark_error_t;

extern "C" void spark_free_error(spark_error_t* error);
extern "C" const char* spark_get_error_message(spark_error_t* error);

namespace spark
{
    namespace client
    {
        struct ThrowOnError
        {
            static
            spark_error_t** get_error_addr()
            {
                static spark_error_t* error = nullptr;
                return &error;
            }

            static void throw_error(spark_error_t* error)
            {
                std::runtime_error ex(spark_get_error_message(error));
                spark_free_error(error);
                throw ex;
            }

            static
             __attribute__ ((noinline))
             void handle_error()
            {
                spark_error_t** pError = get_error_addr();
                spark_error_t* error = *pError;
                if(error)
                {
                    *pError = nullptr;
                    throw_error(error);
                }
            }

            ~ThrowOnError() noexcept(false)
            {
                handle_error();
            }
            operator spark_error_t**() const
            {
                return get_error_addr();
            }
        };

        inline void print_assert(const char* msg, const char* source, uint32_t line)
        {
            printf("%s(%u) : %s\n", source, line, msg);
        }
    }
}