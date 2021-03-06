#pragma once

/*
 * Abstraction for host-side buffers (each backed by a flat array)
 */
 
namespace SiCKL
{
    template<typename T>
    struct DeviceBuffer1D;
    template<typename T>
    struct DeviceBuffer2D;
    
    template<typename T>
    struct HostBuffer1D : public RefCounted<HostBuffer1D<T>>
    {
        REF_COUNTED(HostBuffer1D)
        
        HostBuffer1D()
        : Length(0)
        , BufferSize(0)
        , _buffer(nullptr)
        { }
        
        sickl_int Initialize(cl_uint in_length)
        {
            return Initialize(in_length, nullptr);
        }
        
        sickl_int Initialize(cl_uint in_length, const T* in_buffer)
        {
            ReturnErrorIfFalse(_buffer == nullptr, SICKL_ALREADY_INITIALIZED);
            ReturnErrorIfTrue(in_length == 0, SICKL_INVALID_ARG);
            
            const size_t buffer_size = sizeof(T) * size_t(in_length);
            _buffer = new T[in_length];
            ReturnErrorIfNull(_buffer, SICKL_OUT_OF_MEMORY);
            
            if(in_buffer == nullptr)
            {
                ::memset(_buffer, 0x00, buffer_size);
            }
            else
            {
                ::memcpy(_buffer, in_buffer, buffer_size);
            }
            
            const_cast<cl_uint&>(Length) = in_length;
            const_cast<size_t&>(BufferSize) = buffer_size;
            
            return SICKL_SUCCESS;
        }
        
        sickl_int Initialize(const DeviceBuffer1D<T>& device_buffer)
        {
            ReturnIfError(this->Initialize(device_buffer.Length));
            ReturnIfError(device_buffer.GetData(_buffer));
            
            return SICKL_SUCCESS;
        }
        
        T& operator[](cl_uint index)
        {
            SICKL_ASSERT(index < Length);
            return _buffer[index];
        }
        
        const T& operator[](cl_uint index) const
        {
            SICKL_ASSERT(index < Length);
            return _buffer[index];
        }
        
        operator T*()
        {
            return _buffer;
        }
        
        operator const T*() const
        {
            return _buffer;
        }
        
        const cl_uint Length;
        const size_t BufferSize; 
        
    private:
        T* _buffer;
        
        friend struct DeviceBuffer1D<T>;
    };

    template<typename T>
    void HostBuffer1D<T>::Delete()
    {
        delete[] _buffer;
    }

    template<typename T>
    struct HostBuffer2D : public RefCounted<HostBuffer2D<T>>
    {
        REF_COUNTED(HostBuffer2D)
    
        HostBuffer2D()
        : Width(0)
        , Height(0)
        , BufferSize(0)
        , ElementCount(0)
        , _buffer(nullptr)
        { }
    
        sickl_int Initialize(cl_uint in_width, cl_uint in_height)
        {
            return Initialize(in_width, in_height, nullptr);
        }

        sickl_int Initialize(cl_uint in_width, cl_uint in_height, const T* in_buffer)
        {
            ReturnErrorIfFalse(_buffer == nullptr, SICKL_ALREADY_INITIALIZED);
            ReturnErrorIfTrue(in_width == 0 || in_height == 0, SICKL_INVALID_ARG);
            
            const size_t element_count = size_t(in_width) * size_t(in_height);
            const size_t buffer_size = sizeof(T) * element_count;
        
            _buffer = new T[element_count];
            ReturnErrorIfNull(_buffer, SICKL_OUT_OF_MEMORY);
            
            // init to 0 if nothing given to us
            if(in_buffer == nullptr)
            {
                ::memset(_buffer, 0x00, buffer_size);
            }   
            else
            {
                ::memcpy(_buffer, in_buffer, buffer_size);
            }
            
            const_cast<cl_uint&>(Width) = in_width;
            const_cast<cl_uint&>(Height) = in_height;
            const_cast<size_t&>(BufferSize) = buffer_size;
            const_cast<size_t&>(ElementCount) = element_count;
            
            return SICKL_SUCCESS;
        }
    
        sickl_int Initialize(const DeviceBuffer2D<T>& device_buffer)
        {
            ReturnIfError(this->Initialize(device_buffer.Width, device_buffer.Height));
            ReturnIfError(device_buffer.GetData(_buffer));
            
            return SICKL_SUCCESS;
        }
    
        // allows user to access individual elements via buffer2d[i][j];
        T* operator[](size_t row)
        {
            SICKL_ASSERT(row < Rows);
            return _buffer + row * Width;
        }

        operator T*()
        {
            return _buffer;
        }
        
        operator const T*() const
        {
            return _buffer;
        }
    
        union
        {
            const cl_uint Columns;
            const cl_uint Width;        
        };

        union
        {
            const cl_uint Rows;
            const cl_uint Height;
        };
        
        const size_t BufferSize;
        const size_t ElementCount;

    private:
        T* _buffer;
        
        friend struct DeviceBuffer2D<T>;
    };

    template<typename T>
    void HostBuffer2D<T>::Delete()
    {
        delete[] _buffer;
    }
}
