#include "SiCKL.h"
#include "SiCKLUndef.h"

namespace SiCKL
{
    cl_context Runtime::_context = nullptr;
    cl_device_id Runtime::_device = nullptr;
    cl_command_queue Runtime::_command_queue = nullptr;

    sickl_int Runtime::Initialize()
    {
        cl_int err = CL_SUCCESS;
        cl_platform_id platform_id;

        // get our plafrom id
        err = clGetPlatformIDs(1, &platform_id, nullptr);
        if(err == CL_SUCCESS)
        {
            cl_context_properties properties[] =
            {
                CL_CONTEXT_PLATFORM,
                (cl_context_properties)platform_id,
                0,
            };
            // create our context
            _context = clCreateContextFromType(properties, CL_DEVICE_TYPE_GPU, nullptr, nullptr, &err);

            size_t device_buffer_size;
            err = clGetContextInfo(Runtime::_context, CL_CONTEXT_DEVICES, 0, nullptr, &device_buffer_size);
            if(err == CL_SUCCESS)
            {
                SICKL_ASSERT(device_buffer_size / sizeof(cl_device_id) > 0);
                cl_device_id* devices = new cl_device_id[device_buffer_size / sizeof(cl_device_id)];
                err = clGetContextInfo(Runtime::_context, CL_CONTEXT_DEVICES, device_buffer_size, devices, nullptr);
                if(err == CL_SUCCESS)
                {
                    _command_queue = clCreateCommandQueue(Runtime::_context, devices[0], 0, nullptr);
                    _device = devices[0];
                }
                delete devices;
            }
        }
        return err;
    }

    sickl_int Runtime::Finalize()
    {
        if(_context != nullptr)
        {
            clReleaseContext(_context);
            clReleaseCommandQueue(_command_queue);
        }
        return SICKL_SUCCESS;
    }

   
}
