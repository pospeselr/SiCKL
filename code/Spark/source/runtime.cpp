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
        using unique_cl_context = unique_any<cl_context, decltype(&::clReleaseContext), &::clReleaseContext>;
        using unique_command_queue = unique_any<cl_command_queue, decltype(&::clReleaseCommandQueue), &::clReleaseCommandQueue>;
        using unique_cl_program = unique_any<cl_program, decltype(&::clReleaseProgram), &::clReleaseProgram>;
        using unique_cl_kernel = unique_any<cl_kernel, decltype(&::clReleaseKernel), &::clReleaseKernel>;

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
            this->context.reset(clContext);

            // create command queue
            cl_int createCommandQueueError = CL_SUCCESS;
            cl_command_queue clCommandQueue = ::clCreateCommandQueue(this->context.get(), this->device_id, 0, &createCommandQueueError);
            THROW_IF_OPENCL_FAILED(createCommandQueueError);
            this->command_queue.reset(clCommandQueue);
        }

        struct spark_kernel
        {
            spark_kernel(string&& source);

            string _source;
            unique_cl_program _program;
            unique_cl_kernel _kernel;
        };

        spark_kernel::spark_kernel(string&& source)
        : _source(source)
        {
            auto currentContext = spark_context::current;
            THROW_IF_FALSE(currentContext != nullptr);

            // create program source
            const char* sourceBuffer = this->_source.data();
            const size_t sourceLength = this->_source.size();
            cl_int createProgramWithSourceError = CL_SUCCESS;
            cl_program clProgram = ::clCreateProgramWithSource(currentContext->context.get(), 1, &sourceBuffer, &sourceLength, &createProgramWithSourceError);
            THROW_IF_OPENCL_FAILED(createProgramWithSourceError);
            this->_program.reset(clProgram);

            // build program
            cl_int buildProgramError = ::clBuildProgram(this->_program.get(), 1, &currentContext->device_id, nullptr, nullptr, nullptr);
            if(buildProgramError == CL_BUILD_PROGRAM_FAILURE)
            {
                // get error log message
                size_t logSize;
                THROW_IF_OPENCL_FAILED(::clGetProgramBuildInfo(this->_program.get(), currentContext->device_id, CL_PROGRAM_BUILD_LOG, 0, nullptr, &logSize));

                string logMessage(logSize - 1, 0);
                THROW_IF_OPENCL_FAILED(::clGetProgramBuildInfo(this->_program.get(), currentContext->device_id, CL_PROGRAM_BUILD_LOG, logSize, const_cast<char*>(logMessage.data()), nullptr));

                throw_error(logMessage.c_str(), __FILE__, __LINE__);
            }
            THROW_IF_OPENCL_FAILED(buildProgramError);

            // create kernel with 'main' entrypoint
            cl_int createKernelError = CL_SUCCESS;
            cl_kernel clKernel = ::clCreateKernel(this->_program.get(), "main", &createKernelError);
            THROW_IF_OPENCL_FAILED(createKernelError);
            this->_kernel.reset(clKernel);
        }
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

            THROW_IF_OPENCL_FAILED(clGetDeviceInfo(context->device_id, CL_DEVICE_VERSION, sizeof(buffer), buffer, nullptr));
            printf("OpenCL Version: %s\n", buffer);

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
            auto len = generateSourceTree(kernel_root, nullptr, 0);
            string sourceTree(len - 1, 0);
            generateSourceTree(kernel_root, const_cast<char*>(sourceTree.data()), len);

            printf("%s\n", sourceTree.c_str());

            // generates opencl source from AST
            len = generateOpenCLSource(kernel_root, nullptr, 0);
            string openclSource(len - 1, 0);
            generateOpenCLSource(kernel_root, const_cast<char*>(openclSource.data()), len);

            // build/link kernel
            auto kernel = new spark::lib::spark_kernel(std::move(openclSource));
            return kernel;
        });
}

SPARK_EXPORT const char* spark_get_kernel_source(spark_kernel_t* kernel, spark_error_t** error)
{
    return TranslateExceptions(
        error,
        [&]
        {
            return kernel->_source.c_str();
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