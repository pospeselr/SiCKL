#include "SiCKL.h"
#include "SiCKLUndef.h"

namespace SiCKL
{
    //
    // Program
    //

    Program::Program()
        : _types(nullptr)
        , _type_count(0)
        , _param_index(0)
        , _arg_index(0)
        , _program(nullptr)
        , _kernel(nullptr)
        , _dimension_count(0)
    {
        _work_dimensions[0] = 0;
        _work_dimensions[1] = 0;
        _work_dimensions[2] = 0;
    }

    void Program::Delete()
    {
        if(_kernel != nullptr)
        {
            SICKL_ASSERT(clReleaseKernel(_kernel) == CL_SUCCESS);
            _kernel = nullptr;
        }

        if(_program != nullptr)
        {
            SICKL_ASSERT(clReleaseProgram(_program) == CL_SUCCESS);
            _program = nullptr;
        }
        delete[] _types;
        _types = nullptr;
        _type_count = 0;
        _dimension_count = 0;
        _work_dimensions[0] = 0;
        _work_dimensions[1] = 0;
        _work_dimensions[2] = 0;
    }

    void Program::SetWorkDimensions(size_t length)
    {
        SICKL_ASSERT(length > 0);

        _work_dimensions[0] = length;
        _work_dimensions[1] = _work_dimensions[2] = 0;

        _dimension_count = 1;
    }

    void Program::SetWorkDimensions(size_t length, size_t width)
    {
        SICKL_ASSERT(length > 0);
        SICKL_ASSERT(width > 0);

        _work_dimensions[0] = length;
        _work_dimensions[1] = width;
        _work_dimensions[2] = 0;

        _dimension_count = 2;
    }

    void Program::SetWorkDimensions(size_t length, size_t width, size_t height)
    {
        SICKL_ASSERT(length > 0);
        SICKL_ASSERT(width > 0);
        SICKL_ASSERT(height > 0);

        _work_dimensions[0] = length;
        _work_dimensions[1] = width;
        _work_dimensions[2] = height;

        _dimension_count = 3;
    }

#define VALIDATE_ARG(ARG, TYPE) \
    template<> \
    sickl_int Program::ValidateArg<ARG>(const DataType_t type) \
    { \
        SICKL_ASSERT(type == TYPE); \
        return type == TYPE ? SICKL_SUCCESS : SICKL_INVALID_KERNEL_ARG;\
    }

    // primitive types
    VALIDATE_ARG(bool, DataType::Bool)

    VALIDATE_ARG(cl_int, DataType::Int)
    VALIDATE_ARG(cl_int2, DataType::Int2)

    VALIDATE_ARG(cl_uint, DataType::UInt)
    VALIDATE_ARG(cl_uint2, DataType::UInt2)

    VALIDATE_ARG(cl_float, DataType::Float)
    VALIDATE_ARG(cl_float2, DataType::Float2)

#undef VALIDATE_ARG

// opencl's cl_type3 is typedefed to cl_type4 (same type)
#define VALIDATE_ARG(ARG, TYPEA, TYPEB) \
    template<> \
    sickl_int Program::ValidateArg<ARG>(const DataType_t type) \
    { \
        SICKL_ASSERT(type == TYPEA || type == TYPEB); \
        return type == TYPEA || type == TYPEB ? SICKL_SUCCESS : SICKL_INVALID_KERNEL_ARG;\
    } \

    VALIDATE_ARG(cl_int4, DataType::Int3, DataType::Int4)
    VALIDATE_ARG(cl_uint4, DataType::UInt3, DataType::UInt4)
    VALIDATE_ARG(cl_float4, DataType::Float3, DataType::Float4)

#undef VALIDATE_ARG

    sickl_int Program::Run()
    {
        cl_event event;
        sickl_int err = clEnqueueNDRangeKernel(Runtime::_command_queue, _kernel, _dimension_count, nullptr, _work_dimensions, nullptr, 0, nullptr, &event);
        if(err == SICKL_SUCCESS)
        {
            err = clWaitForEvents(1, &event);
        }
        return err;
    }
}
