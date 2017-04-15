#include <cstdio>
#include <memory>
#include <iostream>
#include <functional>
#include <typeinfo>
#include <vector>

using std::cout;
using std::endl;
using std::unique_ptr;

// spark
#define SPARK_DEBUGU
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
    SPARK_STATIC_ASSERT(is_assignable<rvalue<Int>, Int>() == false);
    // scalar and vector types
    SPARK_STATIC_ASSERT(is_assignable<Int, Int>() == true);
    SPARK_STATIC_ASSERT(is_assignable<const Int, Int>() == false);
    SPARK_STATIC_ASSERT(is_assignable<Int, UInt>() == false);
    SPARK_STATIC_ASSERT(is_assignable<Int2, Int2>() == true);
    SPARK_STATIC_ASSERT(is_assignable<Int2, UInt2>() == false);
    // properties
    SPARK_STATIC_ASSERT(is_assignable<decltype(Int2::X), Int>() == true);
    SPARK_STATIC_ASSERT(is_assignable<decltype(Int2::X), UInt>() == false);
    SPARK_STATIC_ASSERT(is_assignable<decltype(Int2::XY), Int2>() == true);
    SPARK_STATIC_ASSERT(is_assignable<decltype(Int2::XY), UInt2>() == false);
    SPARK_STATIC_ASSERT(is_assignable<decltype(Int2::XX), Int2>() == false);
    SPARK_STATIC_ASSERT(is_assignable<Int, decltype(Int2::X)>() == true);
    SPARK_STATIC_ASSERT(is_assignable<Int2, decltype(Int2::XY)>() == true);
    // casting
    SPARK_STATIC_ASSERT(is_assignable<decltype(Int().As<UInt>()), Int>() == false);
    SPARK_STATIC_ASSERT(is_assignable<decltype(Int().As<UInt>()), UInt>() == false);
    // return from operator
    SPARK_STATIC_ASSERT(is_assignable<decltype(Int() + Int()), Int>() == false);
    // assignment operator
    SPARK_STATIC_ASSERT(is_assignable<decltype(Int() = Int()), Int>() == true);
    // post/pre inc/dec
    SPARK_STATIC_ASSERT(is_assignable<decltype(Int()++), Int>() == false);
    SPARK_STATIC_ASSERT(is_assignable<decltype(Int()--), Int>() == false);
    SPARK_STATIC_ASSERT(is_assignable<decltype(++Int()), Int>() == false);
    SPARK_STATIC_ASSERT(is_assignable<decltype(--Int()), Int>() == false);
    // pointer dereference
    SPARK_STATIC_ASSERT(is_assignable<decltype(*(Pointer<Int>(extern_construct))), Int>() == true);
    SPARK_STATIC_ASSERT(is_assignable<decltype(*(Pointer<Int>(extern_construct))), UInt>() == false);
}

int main()
{
    try
    {
        auto context = spark_create_context(ThrowOnError());
        spark_set_current_context(context, ThrowOnError());

        device_buffer2d<float2> float_table(13, 6, nullptr);

        Kernel<Void(Float2, Float2, Buffer2D<Float2>)> mandelbrot = []()
        {
            auto main = MakeFunction([](Float2 min, Float2 max, Buffer2D<Float2> output)
            {
                output[Index()] = NormalizedIndex();
            });
            main.SetEntryPoint();
        };
        mandelbrot.set_work_dimensions(float_table.rows(), float_table.columns());

        float2 min, max;
        mandelbrot(min, max, float_table);

        unique_ptr<float2[]> host_table(new float2[float_table.count()]);
        float_table.read(host_table.get());

        for(int i = 0; i < float_table.rows(); i++)
        {
            for(int j = 0; j < float_table.columns(); j++)
            {
                const auto& current = host_table[i * float_table.columns() + j];
                cout << "<" << current.x << ", " << current.y << ">, ";
            }
            cout << endl;
        }


#if 0
        Kernel<Void(Int, Buffer1D<Int>)> kernel = []()
        {
            Function<Void(Int, Buffer1D<Int>)> main = [](Int val, Buffer1D<Int> buff)
            {
                Long what = 0;
                what = what + buff.Size;

                buff.Data()[0] = 12;

                buff[42] = 16;
            };
            main.SetEntryPoint();
        };
#endif
#if 0
        Kernel<Void(Buffer1D<Int>, Buffer1D<Float>)> kernel = []()
        {

            auto sum = MakeFunction([](Int a, Int b)
            {
                Comment("Sum");
                a = 12;
                Return(a + b);
            });

            auto square = MakeFunction(
            [](Float val)
            {
                Return(val * val);
            });

            auto main = MakeFunction(
            [=](Buffer1D<Int> buff1, Buffer1D<Float> buff2)
            {
                Comment("Kernel Main");

                Int a = 123;
                Float b = 666.0f;
                Pointer<Int> pA = &a;
                *pA = 14;
                pA[a] = 42;

                Pointer<Int> pAOffset = pA + 14;

                sum(a, b.As<Int>());

                If(b == 123.0f)
                {
                    Comment("Equal!");
                }
                ElseIf(a == 125)
                {
                    Comment("Elseif!");
                }
                Else
                {
                    Comment("Else");
                }
                a = a + 17;

                Comment("Before Dereference");

                buff1[0] = 12;
                Int what = *(buff1.Data() + 1u);
                //printf("typeid: %s\n", typeid(what).name());

                Comment("After Dereference");

                Float2 vec2;
                vec2.X = 1.0f;
                buff2[0] = square(2.0f);
                b = vec2.X;
                Int2 equals = vec2 == vec2;

                Float first = vec2.X;
                vec2.X = 13.6f;
                //first = first * first;

                Float second = vec2.Y;
                vec2.Y = second;
                second = second * second;

                Float aFloat;
                aFloat = 15.0f;

                Float2 vec3 = {first, 112345.0f};
                vec3 = {second, 19.0f};

                While(aFloat == 12.0f)
                {
                    Comment("What");
                    Break();
                    what = equals.Y;
                    ++what;
                }

                vec3.X = 13.2f;
                Int2 swiz = equals.XX;
                swiz.XY = equals.YX;
                Return();
            });
            main.SetEntryPoint();
        };
        kernel.set_work_dimensions(10, 1, 1);
        kernel(int_buffer, float_buffer);

#endif
        spark_destroy_context(context, ThrowOnError());
    }
    catch(std::exception& ex)
    {
        cout << ex.what() << endl;
    }

#if 0
        auto main = make_function<Void(Pointer<Int>, Pointer<Int>)>(
        [](Pointer<Int> a, Pointer<Int> b)
        {
            sum(1, 2);
        });
#endif
#if 0
        Int raw = (666);
        Pointer<Int> pint = &raw;

        raw = (123);

        Int i = 0;

        Comment("Before For");
        For(auto& it : Range<Int>(raw, 1, 13))
        {
            Comment("Body");
            //i = it + 12;

        }
        Comment("After For");
#endif
#if 0
        Comment("Hello Comment");
        auto sum = make_function<Int, Int, Int>(
        [](Int a, Int b)
        {
            Comment("sum -> Int");
            If(a)
            {
                Return(a + b);
            }
            ElseIf(b)
            {
                Return(a + 2 * b);
            }
            Else
            {
                Return(a);
            }

            Return(a + b);
        });

        Comment("Default Constructors");
        Int a, b;
        Comment("Function call");
        Int c = sum(1, b);
        Comment("Copy Constructor");
        Int d = a;
        Float f = sum(1, b).As<Float>();


        Comment("Call sum");
        sum(a, b);
#endif
#if 0
        Int a, b;
        Int c = a + b;

        UInt d = (a + b).As<UInt>();
        Int2 ivec1({12, 27});
        ivec1 + ivec1;
        ivec1 = {13, 61};

        ivec1 = ivec1.XX();
        ivec1.XY = ivec1;

        ivec1[a];
        ivec1[0];

        d = a.As<UInt>();

        a = (d == d);

        Int eq = (c == c);
        UNREFERENCED_PARAMETER(eq);

        Float f = 1.2f;
        -f;

        Float2 fvec1;
        fvec1  = {1.0f, 2.0f};
        ivec1 = (fvec1 == fvec1);


        f = fvec1[a];
        fvec1[a] = f;
        fvec1[0] = 1.0f;

        -ivec1;

        a++;
        a--;
        ++a;
        --a;

        Int inc = a++;
        inc = a--;
        inc = --a;
        inc = ++a;
    });
#endif
    #define PRINT_SIZEOF(TYPE) printf("sizeof(" #TYPE "): %lu\n", sizeof(TYPE))

    PRINT_SIZEOF(spark::Int);
    PRINT_SIZEOF(spark::UInt);
    PRINT_SIZEOF(spark::Int2);

    PRINT_SIZEOF(spark::client::rvalue<spark::Int>);


    #define PRINT_OFFSETOF(TYPE, MEMBER) printf("offsetof(" #TYPE ", " #MEMBER ") : %lu\n", __builtin_offsetof(TYPE, MEMBER))

    return 0;
}

