#include "spark.hpp"

#include "runtime.hpp"
#include "error.hpp"
#include "resource.hpp"

using std::unique_ptr;
using std::make_unique;

// lets us use OpenCL release functions with unique_any type
#pragma GCC diagnostic ignored "-Wignored-attributes"

namespace spark
{
    namespace lib
    {
        using unique_cl_context = unique_any<cl_context, decltype(&clReleaseContext), &clReleaseContext>;
        using unique_command_queue = unique_any<cl_command_queue, decltype(&clReleaseCommandQueue), clReleaseCommandQueue>;

        struct spark_context
        {
            spark_context();

            unique_cl_context context;
            cl_device_id device_id;
            unique_command_queue command_queue;

            static thread_local spark_context* current;
        };
        thread_local spark_context* spark_context::current = nullptr;

        spark_context::spark_context()
        {
            // get our platform id
            cl_platform_id platformId;
            THROW_IF_OPENCL_FAILED(::clGetPlatformIDs(1, &platformId, nullptr));

            // get device id
            THROW_IF_OPENCL_FAILED(::clGetDeviceIDs(platformId, CL_DEVICE_TYPE_GPU, 1, &this->device_id, nullptr));

            // get opencl context
            cl_int createContextError = CL_SUCCESS;
            cl_context clContext = ::clCreateContext(nullptr, 1, &this->device_id, nullptr, nullptr, &createContextError);
            THROW_IF_OPENCL_FAILED(createContextError);
            this->context = unique_cl_context(clContext);

            // create command queue
            cl_int createCommandQueueError = CL_SUCCESS;
            cl_command_queue clCommandQueue = ::clCreateCommandQueue(this->context.get(), this->device_id, 0, &createCommandQueueError);
            THROW_IF_OPENCL_FAILED(createCommandQueueError);
            this->command_queue = unique_command_queue(clCommandQueue);
        }
    }
}

DLL_PUBLIC spark_context_t* spark_create_context(spark_error_t** error)
{
    return TranslateExceptions(
        error,
        [&]
        {
            auto context = new spark::lib::spark_context();
            spark::lib::spark_context::current = context;

            char buffer[1024];

            THROW_IF_OPENCL_FAILED(clGetDeviceInfo(context->device_id, CL_DEVICE_NAME, sizeof(buffer), buffer, nullptr));
            printf("DeviceName: %s\n", buffer);

            THROW_IF_OPENCL_FAILED(clGetDeviceInfo(context->device_id, CL_DEVICE_VENDOR, sizeof(buffer), buffer, nullptr));
            printf("DeviceVendor: %s\n", buffer);

            return spark::lib::spark_context::current;
        });
}

DLL_PUBLIC void spark_set_current_context(spark_context_t* context, spark_error_t** error)
{
    return TranslateExceptions(
        error,
        [&]
        {
            spark::lib::spark_context::current = context;
            return;
        });
}

DLL_PUBLIC void spark_destroy_context(spark_context_t* context, spark_error_t** error)
{
    return TranslateExceptions(
        error,
        [&]
        {
            if(context == spark::lib::spark_context::current)
            {
                spark::lib::spark_context::current = nullptr;
            }
            delete context;
            return;
        });
}