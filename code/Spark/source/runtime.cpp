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
        using unique_cl_mem = unique_any<cl_mem, decltype(&::clReleaseMemObject), &::clReleaseMemObject>;

        /// Spark Context

        struct spark_context
        {
            spark_context();

            unique_cl_context context;
            cl_device_id device_id;
            unique_command_queue command_queue;

            static thread_local spark_context* current;
        };
        thread_local spark_context* spark_context::current = nullptr;

        struct spark_buffer
        {
            spark_buffer(size_t size, const void* data);
            void write(size_t offset, size_t bytes, const void* data);
            void read(size_t offset, size_t bytes, void* dest) const;

            unique_cl_mem _mem;
        };

        struct spark_kernel
        {
            spark_kernel(string&& source);

            void set_arg(uint32_t index, const spark_buffer* buffer);
            void set_arg(uint32_t index, size_t size, const void* data);

            void run(const size_t(&work_dimensions)[3]);

            string _source;
            unique_cl_program _program;
            unique_cl_kernel _kernel;
        };

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

        /// Spark Kernel

        spark_kernel::spark_kernel(string&& source)
        : _source(source)
        {
            auto currentContext = spark_context::current;
            THROW_IF_NULL(currentContext);

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

                // logSize includes null terminator
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

        void spark_kernel::set_arg(uint32_t index, const spark_buffer* buffer)
        {
            auto mem = buffer->_mem.get();
            THROW_IF_OPENCL_FAILED(::clSetKernelArg(this->_kernel.get(), index, sizeof(mem), &mem));
        }

        void spark_kernel::set_arg(uint32_t index, size_t size, const void* data)
        {
            THROW_IF_OPENCL_FAILED(::clSetKernelArg(this->_kernel.get(), index, size, data));
        }

        void spark_kernel::run(const size_t(&work_dimensions)[3])
        {
            auto currentContext = spark_context::current;
            THROW_IF_NULL(currentContext);

            cl_event event;
            THROW_IF_OPENCL_FAILED(::clEnqueueNDRangeKernel(currentContext->command_queue.get(), this->_kernel.get(), 3, nullptr, work_dimensions, nullptr, 0, nullptr, &event));

            THROW_IF_OPENCL_FAILED(::clWaitForEvents(1, &event));
        }

        // Spark Buffer

        spark_buffer::spark_buffer(size_t size, const void* data)
        {
            auto currentContext = spark_context::current;
            THROW_IF_NULL(currentContext);

            cl_int createBufferError = CL_SUCCESS;
            cl_mem_flags memFlags = (data == nullptr) ? CL_MEM_READ_WRITE : (CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
            cl_mem clMem = ::clCreateBuffer(currentContext->context.get(), memFlags, size, const_cast<void*>(data), &createBufferError);
            THROW_IF_OPENCL_FAILED(createBufferError);
            this->_mem.reset(clMem);
        }

        void spark_buffer::write(size_t offset, size_t bytes, const void* data)
        {
            auto currentContext = spark_context::current;
            THROW_IF_NULL(currentContext);

            // copy data
            if(data != nullptr)
            {
                THROW_IF_OPENCL_FAILED(::clEnqueueWriteBuffer(currentContext->command_queue.get(), this->_mem.get(), CL_TRUE, offset, bytes, data, 0, nullptr, nullptr));
            }
            // zero out buffer
            else
            {
                uint8_t zero = 0;
                cl_event event;
                THROW_IF_OPENCL_FAILED(::clEnqueueFillBuffer(currentContext->command_queue.get(), this->_mem.get(), &zero, sizeof(zero), offset, bytes, 0, nullptr, &event));
                THROW_IF_OPENCL_FAILED(::clWaitForEvents(1, &event));
            }
        }

        void spark_buffer::read(size_t offset, size_t bytes, void* dest) const
        {
            auto currentContext = spark_context::current;
            THROW_IF_NULL(currentContext);

            THROW_IF_OPENCL_FAILED(::clEnqueueReadBuffer(currentContext->command_queue.get(), const_cast<cl_mem>(this->_mem.get()), CL_TRUE, offset, bytes, dest, 0, nullptr, nullptr));
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
            THROW_IF_NULL(context);

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
            THROW_IF_NULL(context);

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
            THROW_IF_NULL(kernel_root);

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
            THROW_IF_NULL(kernel);

            return kernel->_source.c_str();
        });
}

SPARK_EXPORT void spark_set_kernel_arg_buffer(spark_kernel_t* kernel, uint32_t index, spark_buffer_t* buffer, spark_error_t** error)
{
    return TranslateExceptions(
        error,
        [&]
        {
            THROW_IF_NULL(kernel);
            THROW_IF_NULL(buffer);

            kernel->set_arg(index, buffer);
            return;
        });
}

SPARK_EXPORT void spark_set_kernel_arg_primitive(spark_kernel_t* kernel, uint32_t index, size_t size, const void* data, spark_error_t** error)
{
    return TranslateExceptions(
        error,
        [&]
        {
            THROW_IF_NULL(kernel);
            THROW_IF_FALSE(size > 0);
            THROW_IF_NULL(data);

            kernel->set_arg(index, size, data);
            return;
        });
}

SPARK_EXPORT void spark_run_kernel(spark_kernel_t* kernel, size_t dim1, size_t dim2, size_t dim3, spark_error_t** error)
{
    return TranslateExceptions(
        error,
        [&]
        {
            THROW_IF_NULL(kernel);
            THROW_IF_FALSE(dim1 > 0);
            THROW_IF_FALSE(dim2 > 0);
            THROW_IF_FALSE(dim3 > 0);

            size_t work_dimensions[3] = {dim1, dim2, dim3};
            kernel->run(work_dimensions);
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

SPARK_EXPORT spark_buffer_t* spark_create_buffer(size_t bytes, const void* data, spark_error_t** error)
{
    return TranslateExceptions(
        error,
        [&]
        {
            return new spark::lib::spark_buffer(bytes, data);
        });
}

SPARK_EXPORT void spark_write_buffer(spark_buffer_t* buffer, size_t offset, size_t bytes, const void* data, spark_error_t** error)
{
    return TranslateExceptions(
    error,
    [&]
    {
        THROW_IF_NULL(buffer);

        buffer->write(offset, bytes, data);
    });
}

SPARK_EXPORT void spark_read_buffer(spark_buffer_t* buffer, size_t offset, size_t bytes, void* dest, spark_error_t** error)
{
    return TranslateExceptions(
    error,
    [&]
    {
        THROW_IF_NULL(buffer);
        THROW_IF_NULL(dest);

        buffer->read(offset, bytes, dest);
    });
}

SPARK_EXPORT void spark_destroy_buffer(spark_buffer_t* buffer, spark_error_t** error)
{
    return TranslateExceptions(
        error,
        [&]
        {
            delete buffer;
            return;
        });
}