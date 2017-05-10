#include <cmath>
#include <cstdio>
#include <memory>
#include <iostream>
#include <functional>
#include <typeinfo>
#include <vector>
#include <set>

using std::cout;
using std::endl;
using std::unique_ptr;

// ruff
#include <ruff.h>

// spark
#define SPARK_NEVER_INLINE RUFF_NEVER_INLINE
#define SPARK_FORCE_INLINE RUFF_FORCE_INLINE
#define SPARK_DEBUGBREAK RUFF_DEBUGBRAEK
#define SPARK_ASSERT(X) RUFF_ASSERT(X)
#define SPARK_VERIFY(X) RUFF_VERIFY(X)

#include <spark.h>
using namespace spark;
using namespace spark::client;

// EasyBMP
#include <EasyBMP.h>

namespace spark
{
    typedef uint8_t true_t;
    typedef uint16_t false_t;

    template<class T> T get();

    template<class L, class R>
    struct is_assignable
    {
        template<class T, class U> static true_t test(char(*)[sizeof(get<T>() = get<U>())]);
        template<class T, class U> static false_t test(...);
        constexpr operator bool() const {return sizeof(true_t) == sizeof(test<L, R>(0));}
    };

    // verification assignment is working as expected

    // rvalue
    RUFF_STATIC_ASSERT(is_assignable<rvalue<Int>, Int>() == false);
    // scalar and vector types
    RUFF_STATIC_ASSERT(is_assignable<Int, Int>() == true);
    RUFF_STATIC_ASSERT(is_assignable<const Int, Int>() == false);
    RUFF_STATIC_ASSERT(is_assignable<Int, UInt>() == false);
    RUFF_STATIC_ASSERT(is_assignable<Int2, Int2>() == true);
    RUFF_STATIC_ASSERT(is_assignable<Int2, UInt2>() == false);
    // properties
    RUFF_STATIC_ASSERT(is_assignable<decltype(Int2::X), Int>() == true);
    RUFF_STATIC_ASSERT(is_assignable<decltype(Int2::X), UInt>() == false);
    RUFF_STATIC_ASSERT(is_assignable<decltype(Int2::XY), Int2>() == true);
    RUFF_STATIC_ASSERT(is_assignable<decltype(Int2::XY), UInt2>() == false);
    RUFF_STATIC_ASSERT(is_assignable<decltype(Int2::XX), Int2>() == false);
    RUFF_STATIC_ASSERT(is_assignable<Int, decltype(Int2::X)>() == true);
    RUFF_STATIC_ASSERT(is_assignable<Int2, decltype(Int2::XY)>() == true);
    // casting
    RUFF_STATIC_ASSERT(is_assignable<decltype(Int().As<UInt>()), Int>() == false);
    RUFF_STATIC_ASSERT(is_assignable<decltype(Int().As<UInt>()), UInt>() == false);
    // return from operator
    RUFF_STATIC_ASSERT(is_assignable<decltype(Int() + Int()), Int>() == false);
    // assignment operator
    RUFF_STATIC_ASSERT(is_assignable<decltype(Int() = Int()), Int>() == true);
    // post/pre inc/dec
    RUFF_STATIC_ASSERT(is_assignable<decltype(Int()++), Int>() == false);
    RUFF_STATIC_ASSERT(is_assignable<decltype(Int()--), Int>() == false);
    RUFF_STATIC_ASSERT(is_assignable<decltype(++Int()), Int>() == false);
    RUFF_STATIC_ASSERT(is_assignable<decltype(--Int()), Int>() == false);
    // pointer dereference
    RUFF_STATIC_ASSERT(is_assignable<decltype(*(Pointer<Int>(extern_construct))), Int>() == true);
    RUFF_STATIC_ASSERT(is_assignable<decltype(*(Pointer<Int>(extern_construct))), UInt>() == false);
}

void make_mandelbrot()
{
    auto context = spark_create_context(SPARK_THROW_ON_ERROR());
    spark_set_current_context(context, SPARK_THROW_ON_ERROR());

    device_buffer2d<uint8_t> device_fractal(2800, 1600, nullptr);
    const int32_t max_iterations = 1024;

    // mandelbrot generating code
    Kernel<Void(Float2, Float2, Buffer2D<UChar>)> mandelbrot = []()
    {
        auto main = MakeFunction([&](Float2 min, Float2 max, Buffer2D<UChar> output)
        {
            Float2 normalized_index = NormalizedIndex();

            Float2 pos0 = normalized_index * (max - min) + min;
            Float2 pos(0.0f, 0.0f);

            Int iteration = 0;
            While(Dot(pos, pos) < 4.0f && iteration < max_iterations)
            {
                Float xtemp = pos.X*pos.X - pos.Y*pos.Y - pos0.X;
                pos.Y = 2.0f * pos.X * pos.Y + pos0.Y;
                pos.X = xtemp;

                iteration++;
            }

            // log scale colour
            Float val = Log1Plus(iteration.As<Float>()) * (255.0f / (float)std::log(max_iterations + 1));
            output[Index()] = (uint8_t)255 - val.As<UChar>();

        });
        main.SetEntryPoint();
    };
    mandelbrot.set_work_dimensions(device_fractal.width(), device_fractal.height());

    // call kernel
    float2 min = {2.5f, 1.0f};
    float2 max = {-1.0f, -1.0f};
    mandelbrot(min, max, device_fractal);

    // bring fractal to device memory
    unique_ptr<uint8_t[]> host_fractal(new uint8_t[device_fractal.count()]);
    device_fractal.read(host_fractal.get());

    // write to bmp
    BMP bmp_fractal;
    bmp_fractal.SetSize(device_fractal.width(), device_fractal.height());

    for(int32_t i = 0; i < device_fractal.height(); i++)
    {
        for(int32_t j = 0; j < device_fractal.width(); j++)
        {
            const auto offset = i * device_fractal.width() + j;
            uint8_t color = host_fractal[offset];

            auto pixel = bmp_fractal(j, i);
            pixel->Red = pixel->Green = pixel->Blue = color;
        }
    }

    bmp_fractal.WriteToFile("fractal.bmp");

}

void verify_buffer_view1d()
{
    const size_t count = 100;
    const size_t half_count = count / 2;
    unique_ptr<int32_t[]> index_buffer(new int32_t[count]);
    for(size_t k = 0; k < count; k++)
    {
        index_buffer[k] = k;
    }

    device_buffer1d<int32_t> indices(count, index_buffer.get());
    device_buffer1d<int32_t> evens(half_count, nullptr);
    device_buffer1d<int32_t> odds(half_count, nullptr);

    Kernel<Void(BufferView1D<Int>, BufferView1D<Int>, BufferView1D<Int>)> split_data = []()
    {
        auto main = MakeFunction([](BufferView1D<Int> source, BufferView1D<Int> even_dest, BufferView1D<Int> odd_dest)
        {
            BufferView1D<Int> even_view(source, half_count, 2, 0);
            BufferView1D<Int> odd_view(source, half_count, 2, 1);

            Int idx = Index().X;

            even_dest[idx] = even_view[idx];
            odd_dest[idx] = odd_view[idx];
        });
        main.SetEntryPoint();
    };

    split_data.set_work_dimensions(half_count);
    split_data(indices, evens, odds);

    int32_t even_buffer[half_count];
    int32_t odd_buffer[half_count];

    evens.read(even_buffer);
    odds.read(odd_buffer);

    for(size_t k = 0; k < half_count; k++)
    {
        SPARK_ASSERT(even_buffer[k] == k * 2);
        SPARK_ASSERT(odd_buffer[k] == k * 2 + 1);
    }
}

#define RUN_TEST(X) current_test = #X; if(tests.find(current_test) != tests.end() || tests.size() == 0) X();

int main(int argc, char** argv)
{
    std::set<std::string> tests;
    for(int k = 1; k < argc; k++)
    {
        tests.insert(argv[k]);
    }

    const char* current_test = nullptr;
    try
    {
        auto context = spark_create_context(SPARK_THROW_ON_ERROR());
        spark_set_current_context(context, SPARK_THROW_ON_ERROR());

        RUN_TEST(make_mandelbrot);
        RUN_TEST(verify_buffer_view1d);

        // end spark session
        spark_destroy_context(context, SPARK_THROW_ON_ERROR());
    }
    catch(std::exception& ex)
    {
        cout << "Exception Thrown on test " << current_test << ":\n" << ex.what() << endl;
        return -1;
    }
    return 0;
}

