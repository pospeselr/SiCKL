#pragma once

#define THROW_IF_OPENCL_FAILED(X)\
    do\
    {\
        cl_int clerr = (X);\
        if(clerr != CL_SUCCESS)\
        {\
            spark::lib::throw_opencl_error(#X, clerr, __FILE__, __LINE__);\
        }\
    }\
    while(0)

#define THROW_IF_FALSE(X)\
    do\
    {\
        auto assertion = (X);\
        if(!assertion)\
        {\
            spark::lib::throw_error(#X, __FILE__, __LINE__);\
        }\
    }\
    while(0)

#define SPARK_ASSERT(X) THROW_IF_FALSE(X)

namespace spark
{
    namespace lib
    {
        struct spark_error;

        void throw_opencl_error(const char* action, cl_int err, const char* source, uint32_t line);
        void throw_error(const char* error, const char* source, uint32_t line);

        extern char g_errorMessage[1024];
        extern void HandleException(spark_error** error, const std::exception& ex);

        template<typename FUNC>
        auto TranslateExceptions(spark_error** error, FUNC&& func) noexcept
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
    }
}

typedef struct spark::lib::spark_error spark_error_t;
