#include <cstdio>
#include <memory>
#include <iostream>
#include <functional>
#include <typeinfo>

using std::cout;
using std::endl;
using std::unique_ptr;

#include <CL/cl.h>

// spark
#include <spark.h>
using namespace Spark;

namespace Spark
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
    // vector index
    SPARK_STATIC_ASSERT(is_assignable<decltype(Int2()[0]), Int>() == true);
    SPARK_STATIC_ASSERT(is_assignable<decltype(Int2()[0]), UInt>() == false);
    SPARK_STATIC_ASSERT(is_assignable<decltype(get<const Int2>()[0]), Int>() == false);
    SPARK_STATIC_ASSERT(is_assignable<decltype((Int2() + Int2())[0]), Int>() == false);
    // pointer dereference
    SPARK_STATIC_ASSERT(is_assignable<decltype(*(Pointer<Int>(nullptr))), Int>() == true);
    SPARK_STATIC_ASSERT(is_assignable<decltype(*(Pointer<Int>(nullptr))), UInt>() == false);
}



int main()
{
    auto cleanup = Spark::scope_exit(::spark_end_program);

    Kernel<void(PInt, PInt)> kernel = []()
    {
        Function<Int(Int,Int)> sum =
        [](Int a, Int b)
        {
            Comment("Sum");
            Return(a + b);
        };

        Function<void(PInt, PInt)> main =
        [=](PInt buff1, PInt buff2)
        {
            Comment("Kernel Main");

            Int a = 123;
            Float b = 666.0f;

            sum(a, b.As<Int>());

            buff1 = 2u + buff1 + 2u;

            Comment("Before Dereference");

            *buff1 = 12;
            Int what = *(buff1 + 1u);
            //printf("typeid: %s\n", typeid(what).name());

            Comment("After Dereference");

            Float2 vec2;
            vec2.X = 1.0f;
            b = vec2.X;
        };
        main.SetEntryPoint();
    };


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

    PRINT_SIZEOF(Spark::Int);
    PRINT_SIZEOF(Spark::UInt);
    PRINT_SIZEOF(Spark::Int2);

    PRINT_SIZEOF(Spark::rvalue<Spark::Int>);


    #define PRINT_OFFSETOF(TYPE, MEMBER) printf("offsetof(" #TYPE ", " #MEMBER ") : %lu\n", __builtin_offsetof(TYPE, MEMBER))

    char prop[16] = {0};
    cout << spark_property_to_str(Property::X, prop, countof(prop)) << endl;
    cout << spark_property_to_str(Property::XX, prop, countof(prop)) << endl;
    cout << spark_property_to_str(Property::WX, prop, countof(prop)) << endl;
    cout << spark_property_to_str(Property::YXWX, prop, countof(prop)) << endl;
    cout << spark_property_to_str(Property::X, prop, countof(prop)) << endl;
    cout << spark_property_to_str(Property::Lo, prop, countof(prop)) << endl;

    try
    {
        spark_test_error(Spark::ThrowOnError());
    }
    catch(std::exception& ex)
    {
        cout << ex.what() << endl;
    }
    return 0;
}

