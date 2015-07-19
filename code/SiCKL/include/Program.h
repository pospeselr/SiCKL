#pragma once

namespace SiCKL
{
    struct Program : public RefCounted<Program>
    {
        REF_COUNTED(Program)
    public:
        Program();

        void SetWorkDimensions(size_t length);
        void SetWorkDimensions(size_t length, size_t width);
        void SetWorkDimensions(size_t length, size_t width, size_t height);

        template<typename...Args>
        sickl_int operator()(const Args&... args)
        {
            _param_index = 0;
            _arg_index = 0;
            sickl_int result =  Run(args...);
            return result;
        }
    private:
        template<typename T>
        sickl_int ValidateArg(const DataType_t);

        template<typename BUFF>
        sickl_int ValidateBufferArg(const DataType_t type)
        {
            SICKL_ASSERT(type & BUFF::BufferType);
            ReturnErrorIfFalse(type & BUFF::BufferType, SICKL_INVALID_KERNEL_ARG);

            return ValidateArg<typename BUFF::InternalType>((const DataType_t)(type ^ BUFF::BufferType));
        }

        template<typename T>
        sickl_int SetArg(const T& arg)
        {
            return clSetKernelArg(_kernel, _arg_index++, sizeof(T), &arg);
        }

        sickl_int Run();

        // simple implementation for primitive types
        template<typename Arg, typename...Args>
        sickl_int Run(const Arg& arg, const Args&... args)
        {;
            DataType_t type = _types[_param_index];
            // make sure this arg matches the required type
            SICKL_ASSERT(ValidateArg<Arg>(type) == SICKL_SUCCESS);

            // pass the arg to our kernel
            ReturnIfError(SetArg(arg));

            ++_param_index;
            ReturnIfError(Run(args...));

            return SICKL_SUCCESS;
        }

        // special implementation for DeviceBuffer1D
        template<typename T, typename...Args>
        sickl_int Run(const DeviceBuffer1D<T>& arg, const Args&... args)
        {
            DataType_t type = _types[_param_index];
            SICKL_ASSERT(ValidateBufferArg<DeviceBuffer1D<T>>(type) == SICKL_SUCCESS);

            ReturnIfError(SetArg(arg.Length));
            ReturnIfError(SetArg(arg._memory_object));

            ++_param_index;
            return Run(args...);
        }

        // special implementation for DeviceBuffer2D
        template<typename T, typename...Args>
        sickl_int Run(const DeviceBuffer2D<T>& arg, const Args&... args)
        {
            DataType_t type = _types[_param_index];
            // makes ure this arg matches the required type
            SICKL_ASSERT(ValidateBufferArg<DeviceBuffer2D<T>>(type) == SICKL_SUCCESS);

            ReturnIfError(SetArg(arg.Width));
            ReturnIfError(SetArg(arg.Height));
            ReturnIfError(SetArg(arg._memory_object));

            ++_param_index;
            return Run(args...);
        }

        // used to ensure our passed in args match the required types
        DataType_t* _types;
        size_t _type_count;
        // counter used in Run(...)
        size_t _param_index;
        // counter used in SetArg
        cl_uint _arg_index;

        // program we built
        cl_program _program;
        // our built kernel
        cl_kernel _kernel;

        // work diemnsions
        size_t _work_dimensions[3];
        cl_uint _dimension_count;

        friend class Compiler;
    };
}
