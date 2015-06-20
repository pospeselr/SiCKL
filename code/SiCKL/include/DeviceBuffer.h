#pragma once

/*
 * Abstraction for a server side clMemoryObject
 */

namespace SiCKL
{
    template<typename T>
    struct HostBuffer1D;
    
    template<typename T>
    struct HostBuffer2D;
    
    template<typename T>
    struct DeviceBuffer1D : public RefCounted<DeviceBuffer1D<T>>
    {
        REF_COUNTED(DeviceBuffer1D)
        
        typedef T InternalType;
        
        DeviceBuffer1D()
        : Length(0)
        , BufferSize(0)
        , _memory_object(nullptr)
        { }
        
        sickl_int Initialize(size_t in_length)
        {
            return Initialize(in_length, nullptr);
        }
        
        sickl_int Initialize(HostBuffer1D<T>& in_host_buffer)
        {
            ReturnErrorIfNull(in_host_buffer._buffer, SICKL_INVALID_ARG);
            
            return Initialize(in_host_buffer.Length, in_host_buffer._buffer);
        }
        
        sickl_int Initialize(size_t in_length, const T* data)
        {
            ReturnErrorIfFalse(_memory_object == nullptr, SICKL_ALREADY_INITIALIZED);
            ReturnErrorIfTrue(in_length == 0, SICKL_INVALID_ARG);
            
            cl_int err = CL_SUCCESS;
            const size_t buffer_size = in_length * sizeof(T);
            
            if(data == nullptr)
            {
                _memory_object = clCreateBuffer(Runtime::_context, CL_MEM_READ_WRITE, buffer_size, nullptr, &err);
            }
            else
            {
                _memory_object = clCreateBuffer(Runtime::_context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, buffer_size, const_cast<T*>(data), &err);
            } 
            
            if(err == CL_SUCCESS)
            {
                const_cast<cl_ulong&>(Length) = in_length;
                const_cast<size_t&>(BufferSize) = buffer_size;
            }
            return err;
        }
        
        sickl_int SetData(const T* in_data)
        {
            ReturnErrorIfNull(in_data, SICKL_INVALID_ARG);
            
            return clEnqueueWriteBuffer(Runtime::_command_queue, _memory_object, true, 0, BufferSize, in_data, 0, nullptr, nullptr);
        }
        
        sickl_int SetData(const HostBuffer1D<T>& in_host_buffer)
        {
            ReturnErrorIfNull(in_host_buffer._buffer, SICKL_INVALID_ARG);
            ReturnErrorIfTrue(in_host_buffer.Length != Length, SICKL_INVALID_ARG);
            
            return SetData(in_host_buffer._buffer);
        }
        
        sickl_int GetData(T* in_buffer) const
        {
            ReturnErrorIfNull(in_buffer, SICKL_INVALID_ARG);
            
            return clEnqueueReadBuffer(Runtime::_command_queue, _memory_object, true, 0, BufferSize, in_buffer, 0, nullptr, nullptr);
        }
        
        sickl_int GetData(HostBuffer1D<T>& in_host_buffer) const
        {
            ReturnErrorIfNull(in_host_buffer._buffer, SICKL_INVALID_ARG);
            ReturnErrorIfTrue(in_host_buffer.Length != Length, SICKL_INVALID_ARG);
            
            return GetData(in_host_buffer._buffer);
        }
        
        const cl_ulong Length;
        const size_t BufferSize;
        
    private:
        cl_mem _memory_object;
        
        const static ReturnType_t BufferType = ReturnType::Buffer1D;
        
        friend struct Program;
    };
    template<typename T>
    void DeviceBuffer1D<T>::Delete()
    {
        clReleaseMemObject(_memory_object);
    }
    
    template<typename T>
    struct DeviceBuffer2D : public RefCounted<DeviceBuffer2D<T>>
    {
        REF_COUNTED(DeviceBuffer2D)
        
        typedef T InternalType;
        
        DeviceBuffer2D()
        : Columns(0)
        , Rows(0)
        , BufferSize(0)
        , ElementCount(0)
        , _memory_object(nullptr)
        { }
        
        sickl_int Initialize(size_t in_width, size_t in_height)
        {
            return Initialize(in_width, in_height, nullptr);
        }
        
        sickl_int Initialize(HostBuffer2D<T>& in_host_buffer)
        {
            ReturnErrorIfNull(in_host_buffer._buffer, SICKL_INVALID_ARG);
            
            return Initialize(in_host_buffer.Width, in_host_buffer.Height, in_host_buffer._buffer);
        }
        
        sickl_int Initialize(size_t in_width, size_t in_height, const T* data)
        {
            ReturnErrorIfFalse(_memory_object == nullptr, SICKL_ALREADY_INITIALIZED);
            ReturnErrorIfTrue(in_width == 0 || in_height == 0, SICKL_INVALID_ARG);
            
            cl_int err = CL_SUCCESS;
            const size_t element_count = in_width * in_height;
            const size_t buffer_size = element_count * sizeof(T);
            
            if(data == nullptr)
            {
                _memory_object = clCreateBuffer(Runtime::_context, CL_MEM_READ_WRITE, buffer_size, nullptr, &err);
            }
            else
            {
                _memory_object = clCreateBuffer(Runtime::_context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, buffer_size, const_cast<T*>(data), &err);
            }
                       
            if(err == CL_SUCCESS)
            {
                const_cast<cl_ulong&>(Width) = in_width;
                const_cast<cl_ulong&>(Height) = in_height;
                const_cast<size_t&>(BufferSize) = buffer_size;
                const_cast<size_t&>(ElementCount) = element_count;
            }
            
            return err;
        }

        sickl_int SetData(const T* in_data)
        {
            ReturnErrorIfNull(in_data, SICKL_INVALID_ARG);
            
            return clEnqueueWriteBuffer(Runtime::_command_queue, _memory_object, true, 0, BufferSize, in_data, 0, nullptr, nullptr);
        }

        sickl_int SetData(const HostBuffer2D<T>& in_host_buffer)
        {
            ReturnErrorIfNull(in_host_buffer._buffer, SICKL_INVALID_ARG);
            ReturnErrorIfTrue(in_host_buffer.Width != Width || in_host_buffer.Height != Height, SICKL_INVALID_ARG);
            
            return SetData(in_host_buffer._buffer);
        }
        
        sickl_int GetData(T* in_buffer) const
        {
            ReturnErrorIfNull(in_buffer, SICKL_INVALID_ARG);
            
            return clEnqueueReadBuffer(Runtime::_command_queue, _memory_object, true, 0, BufferSize, in_buffer, 0, nullptr, nullptr);
        }
        
        sickl_int GetData(HostBuffer2D<T>& in_host_buffer) const
        {
            ReturnErrorIfNull(in_host_buffer._buffer, SICKL_INVALID_ARG);
            ReturnErrorIfTrue(in_host_buffer.Width != Width || in_host_buffer.Height != Height, SICKL_INVALID_ARG);
            
            return GetData(in_host_buffer._buffer);
        }
        
        union
        {
            const cl_ulong Columns;
            const cl_ulong Width;        
        };

        union
        {
            const cl_ulong Rows;
            const cl_ulong Height;
        };
        
        const size_t BufferSize;
        const size_t ElementCount;
        
    private:
        cl_mem _memory_object;
        
        const static ReturnType_t BufferType = ReturnType::Buffer2D;
    
        friend struct Program;  
    };

    template<typename T>
    void DeviceBuffer2D<T>::Delete()
    {
        clReleaseMemObject(_memory_object);
    }
}
