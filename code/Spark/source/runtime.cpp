#include "spark.hpp"

#include "runtime.hpp"
#include "error.hpp"
#include "resource.hpp"
#include "node.hpp"
#include "codegen.hpp"

using std::string;
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

        struct spark_kernel
        {
            std::string kernel_source;
        };
    }
}

SPARK_EXPORT spark_context_t* spark_create_context(spark_error_t** error)
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

SPARK_EXPORT void spark_set_current_context(spark_context_t* context, spark_error_t** error)
{
    return TranslateExceptions(
        error,
        [&]
        {
            spark::lib::spark_context::current = context;
            return;
        });
}

SPARK_EXPORT void spark_destroy_context(spark_context_t* context, spark_error_t** error)
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

SPARK_EXPORT spark_kernel_t* spark_create_kernel(spark_node_t* kernel_root, spark_error_t** error)
{
    return TranslateExceptions(
        error,
        [&]
        {
            auto len = generateOpenCLSource(kernel_root, nullptr, 0);
            string buffer(len - 1, 0);
            generateOpenCLSource(kernel_root, const_cast<char*>(buffer.data()), len);

            unique_ptr<spark_kernel_t> result(new spark::lib::spark_kernel());
            result->kernel_source = std::move(buffer);

            return result.release();
        });
}

SPARK_EXPORT const char* spark_get_kernel_source(spark_kernel_t* kernel, spark_error_t** error)
{
    return TranslateExceptions(
        error,
        [&]
        {
            return kernel->kernel_source.c_str();
        });
}

SPARK_EXPORT void spark_destroy_kernel(spark_kernel_t* kernel, spark_error_t** error)
{
    return TranslateExceptions(
        error,
        [&]
        {
            delete kernel;
            return;
        });
}