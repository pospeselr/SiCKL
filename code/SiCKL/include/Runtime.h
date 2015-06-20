#pragma once

namespace SiCKL
{
    class Runtime
    {
    public:
        // setup opencl runtime
        static sickl_int Initialize();
        // tear it down
        static sickl_int Finalize();
    private:
        template<typename T>
        friend struct DeviceBuffer1D;
        template<typename T>
        friend struct DeviceBuffer2D;
        friend struct Program;
        friend class Compiler;

        static cl_context _context;
        static cl_device_id _device;
        static cl_command_queue _command_queue;
    };
}
