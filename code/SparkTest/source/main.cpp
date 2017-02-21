#include <cstdio>
#include <memory>
#include <iostream>
#include <functional>
#include <typeinfo>

using std::cout;
using std::endl;
using std::unique_ptr;

// spark
#define SPARK_DEBUGU
#include <spark.h>
using namespace spark;
using namespace spark::client;

namespace spark
{
    typedef uint8_t true_t;
    typedef uint16_t false_t;

    template<class T> T& get_lvalue();
    template<class T> T get_rvalue();
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
#if 1
        Kernel<Void(PInt, PFloat)> kernel = []()
        {

            Function<Long(Long,Long)> sum =
            [](Long a, Long b)
            {
                Comment("Sum");
                a = 12;
                Return(a + b);
            };
            Function<Float(Float)> square =
            [](Float val)
            {
                Return(val * val);
            };

            Function<Void(PInt, PFloat)> main =
            [=](PInt buff1, PFloat buff2)
            {
                Comment("Kernel Main");

                Long a = 123;
                Float b = 666.0f;
                Pointer<Long> pA = &a;
                *pA = 14;
                pA[a] = 42;

                Pointer<Long> pAOffset = pA + 14;

                sum(a, b.As<Long>());

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

                buff1 = 7u + buff1 + 2u;

                Comment("Before Dereference");

                *buff1 = 12;
                Int what = *(buff1 + 1u);
                //printf("typeid: %s\n", typeid(what).name());

                Comment("After Dereference");

                Float2 vec2;
                vec2.X = 1.0f;
                *buff2 = square(2.0f);
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
            };
            main.SetEntryPoint();
        };
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

