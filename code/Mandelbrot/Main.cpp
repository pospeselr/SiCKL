#include <SiCKL.h>

using namespace SiCKL;
#include "EasyBMP.h"
#include <math.h>

// algorithm from: http://en.wikipedia.org/wiki/Mandelbrot_set
class Mandelbrot : public Source
{
public:
    Mandelbrot() : max_iterations(512) {}
    const int32_t max_iterations;

    BEGIN_SOURCE
        KernelMain(Float2 min, 
                   Float2 max, 
                   Buffer1D<Float3> color_map, 
                   Buffer2D<Float3> output)
        {
            UInt3 index3 = Index();
            UInt2 index(index3.X, index3.Y);
            
            Float3 ni3 = NormalizedIndex();
            Float2 pos0 = Float2(ni3.X, ni3.Y) * (max -  min) + min;

            Float x0 = pos0.X;
            Float y0 = pos0.Y;

            Float x = 0;
            Float y = 0;

            Int iteration = 0;
            
            While(x*x + y*y < 4.0f && iteration < max_iterations)
            {
                Float xtemp = x*x - y*y + x0;
                y = 2.0f*x*y + y0;

                x = xtemp;

                iteration = iteration + 1;
            }
            
            // log scale iteration count to 0,1
            Float norm_val = Log(((Float)iteration + 1.0f))/float(log(max_iterations + 1.0));

            // get color from lookup buffer
            output(index) = color_map((Int)(norm_val * (float)(max_iterations - 1)));
        }
    END_SOURCE
};

int main()
{
    if(Runtime::Initialize() != SICKL_SUCCESS)
    {
        printf("Could not initialize OpenCL Context\n");
        return -1;
    }

    Mandelbrot mbrot;
    Program program;
    Compiler::Build(mbrot, program);

    const uint32_t width = 350 * 5;
    const uint32_t height = 200 * 5;
    const uint32_t color_count = mbrot.max_iterations;

    /// Generate the color table (a nice gold)
    HostBuffer1D<cl_float3> color_map;
    color_map.Initialize(color_count);

    for(uint32_t i = 0; i < color_map.Length; i++)
    {
        float x = i/(float)color_count;
        cl_float3& currentColor = color_map[i];
        
        currentColor.s[0] = 191.0f / 255.0f * (1.0f - x);
        currentColor.s[1] = 125.0f / 255.0f * (1.0f - x);
        currentColor.s[2] = 37.0f / 255.0f * (1.0f - x);
    }
    
    /// Copy data to device
    DeviceBuffer1D<cl_float3> device_color_map;
    device_color_map.Initialize(color_map);
    
    DeviceBuffer2D<cl_float3> device_image;
    device_image.Initialize(width, height);

    cl_float2 min_loc = {{-2.5f, -1.0f}};
    cl_float2 max_loc = {{1.0f, 1.0f}};
    
    program.SetWorkDimensions(width, height);
    program(min_loc, max_loc, device_color_map, device_image);

    // converting to bmp
    
    HostBuffer2D<cl_float3> host_image;
    SICKL_ASSERT(host_image.Initialize(width, height) == SICKL_SUCCESS);
    SICKL_ASSERT(device_image.GetData(host_image) == SICKL_SUCCESS);
    
     /// Finally, dump the image to a Bitmap to view
    BMP bmp;
    bmp.SetSize(width, height);

    for(uint32_t i = 0; i < host_image.Height; i++)
    {
        for(uint32_t j = 0; j < host_image.Width; j++)
        {
            const cl_float3& currentColor = host_image[i][j];
        
            const float& red = currentColor.s[0];
            const float& green = currentColor.s[1];
            const float& blue = currentColor.s[2];

            auto pixel = bmp(j,i);
            pixel->Red = (uint8_t)(red * 255);
            pixel->Green = (uint8_t)(green * 255);
            pixel->Blue = (uint8_t)(blue * 255);
        }
    }

    bmp.WriteToFile("open_cl.result.bmp");
}
