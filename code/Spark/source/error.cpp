#include "spark.hpp"

#include "error.hpp"
#include "text_utilities.hpp"

namespace spark
{
    namespace lib
    {
        const char* get_cl_error(cl_int err)
        {
            switch(err)
            {
                case CL_SUCCESS:
                    return "CL_SUCCESS";
                case CL_DEVICE_NOT_FOUND:
                    return "CL_DEVICE_NOT_FOUND";
                case CL_DEVICE_NOT_AVAILABLE:
                    return "CL_DEVICE_NOT_AVAILABLE";
                case CL_COMPILER_NOT_AVAILABLE:
                    return "CL_COMPILER_NOT_AVAILABLE";
                case CL_MEM_OBJECT_ALLOCATION_FAILURE:
                    return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
                case CL_OUT_OF_RESOURCES:
                    return "CL_OUT_OF_RESOURCES";
                case CL_OUT_OF_HOST_MEMORY:
                    return "CL_OUT_OF_HOST_MEMORY";
                case CL_PROFILING_INFO_NOT_AVAILABLE:
                    return "CL_PROFILING_INFO_NOT_AVAILABLE";
                case CL_MEM_COPY_OVERLAP:
                    return "CL_MEM_COPY_OVERLAP";
                case CL_IMAGE_FORMAT_MISMATCH:
                    return "CL_IMAGE_FORMAT_MISMATCH";
                case CL_IMAGE_FORMAT_NOT_SUPPORTED:
                    return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
                case CL_BUILD_PROGRAM_FAILURE:
                    return "CL_BUILD_PROGRAM_FAILURE";
                case CL_MAP_FAILURE:
                    return "CL_MAP_FAILURE";
                case CL_MISALIGNED_SUB_BUFFER_OFFSET:
                    return "CL_MISALIGNED_SUB_BUFFER_OFFSET";
                case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST:
                    return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
                case CL_COMPILE_PROGRAM_FAILURE:
                    return "CL_COMPILE_PROGRAM_FAILURE";
                case CL_LINKER_NOT_AVAILABLE:
                    return "CL_LINKER_NOT_AVAILABLE";
                case CL_LINK_PROGRAM_FAILURE:
                    return "CL_LINK_PROGRAM_FAILURE";
                case CL_DEVICE_PARTITION_FAILED:
                    return "CL_DEVICE_PARTITION_FAILED";
                case CL_KERNEL_ARG_INFO_NOT_AVAILABLE:
                    return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";
                case CL_INVALID_VALUE:
                    return "CL_INVALID_VALUE";
                case CL_INVALID_DEVICE_TYPE:
                    return "CL_INVALID_DEVICE_TYPE";
                case CL_INVALID_PLATFORM:
                    return "CL_INVALID_PLATFORM";
                case CL_INVALID_DEVICE:
                    return "CL_INVALID_DEVICE";
                case CL_INVALID_CONTEXT:
                    return "CL_INVALID_CONTEXT";
                case CL_INVALID_QUEUE_PROPERTIES:
                    return "CL_INVALID_QUEUE_PROPERTIES";
                case CL_INVALID_COMMAND_QUEUE:
                    return "CL_INVALID_COMMAND_QUEUE";
                case CL_INVALID_HOST_PTR:
                    return "CL_INVALID_HOST_PTR";
                case CL_INVALID_MEM_OBJECT:
                    return "CL_INVALID_MEM_OBJECT";
                case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
                    return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
                case CL_INVALID_IMAGE_SIZE:
                    return "CL_INVALID_IMAGE_SIZE";
                case CL_INVALID_SAMPLER:
                    return "CL_INVALID_SAMPLER";
                case CL_INVALID_BINARY:
                    return "CL_INVALID_BINARY";
                case CL_INVALID_BUILD_OPTIONS:
                    return "CL_INVALID_BUILD_OPTIONS";
                case CL_INVALID_PROGRAM:
                    return "CL_INVALID_PROGRAM";
                case CL_INVALID_PROGRAM_EXECUTABLE:
                    return "CL_INVALID_PROGRAM_EXECUTABLE";
                case CL_INVALID_KERNEL_NAME:
                    return "CL_INVALID_KERNEL_NAME";
                case CL_INVALID_KERNEL_DEFINITION:
                    return "CL_INVALID_KERNEL_DEFINITION";
                case CL_INVALID_KERNEL:
                    return "CL_INVALID_KERNEL";
                case CL_INVALID_ARG_INDEX:
                    return "CL_INVALID_ARG_INDEX";
                case CL_INVALID_ARG_VALUE:
                    return "CL_INVALID_ARG_VALUE";
                case CL_INVALID_ARG_SIZE:
                    return "CL_INVALID_ARG_SIZE";
                case CL_INVALID_KERNEL_ARGS:
                    return "CL_INVALID_KERNEL_ARGS";
                case CL_INVALID_WORK_DIMENSION:
                    return "CL_INVALID_WORK_DIMENSION";
                case CL_INVALID_WORK_GROUP_SIZE:
                    return "CL_INVALID_WORK_GROUP_SIZE";
                case CL_INVALID_WORK_ITEM_SIZE:
                    return "CL_INVALID_WORK_ITEM_SIZE";
                case CL_INVALID_GLOBAL_OFFSET:
                    return "CL_INVALID_GLOBAL_OFFSET";
                case CL_INVALID_EVENT_WAIT_LIST:
                    return "CL_INVALID_EVENT_WAIT_LIST";
                case CL_INVALID_EVENT:
                    return "CL_INVALID_EVENT";
                case CL_INVALID_OPERATION:
                    return "CL_INVALID_OPERATION";
                case CL_INVALID_GL_OBJECT:
                    return "CL_INVALID_GL_OBJECT";
                case CL_INVALID_BUFFER_SIZE:
                    return "CL_INVALID_BUFFER_SIZE";
                case CL_INVALID_MIP_LEVEL:
                    return "CL_INVALID_MIP_LEVEL";
                case CL_INVALID_GLOBAL_WORK_SIZE:
                    return "CL_INVALID_GLOBAL_WORK_SIZE";
                case CL_INVALID_PROPERTY:
                    return "CL_INVALID_PROPERTY";
                case CL_INVALID_IMAGE_DESCRIPTOR:
                    return "CL_INVALID_IMAGE_DESCRIPTOR";
                case CL_INVALID_COMPILER_OPTIONS:
                    return "CL_INVALID_COMPILER_OPTIONS";
                case CL_INVALID_LINKER_OPTIONS:
                    return "CL_INVALID_LINKER_OPTIONS";
                case CL_INVALID_DEVICE_PARTITION_COUNT:
                    return "CL_INVALID_DEVICE_PARTITION_COUNT";
                case CL_INVALID_PIPE_SIZE:
                    return "CL_INVALID_PIPE_SIZE";
                case CL_INVALID_DEVICE_QUEUE:
                    return "CL_INVALID_DEVICE_QUEUE";
                default:
                    return "UNKNOWN_ERROR";
            }
        }

        void throw_error(const char* error, const char* source, uint32_t line)
        {
            auto buffer = string_format("%s(%u) : %s", source, line, error);
            throw std::runtime_error(buffer);
        }

        void throw_opencl_error(const char* action, cl_int err, const char* source, uint32_t line)
        {
            auto buffer = string_format("%s returned %s", action, get_cl_error(err));
            throw_error(buffer.c_str(), source, line);
        }

        struct spark_error
        {
            spark_error(const char* msg) : _error_message(msg) {}
            std::string _error_message;
        };

        void handle_exception(spark_error_t** error, const std::exception& ex)
        {
            if(error)
            {
                *error = new spark_error(ex.what());
            }
        }
    }
}

RUFF_EXPORT void spark_destroy_error(spark_error_t* error)
{
    delete error;
}

RUFF_EXPORT const char* spark_get_error_message(spark_error_t* error)
{
    return error->_error_message.c_str();
}

