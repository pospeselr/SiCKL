#include "precomp.h"

#include <cstdio>
#include <memory>
#include <iostream>
using namespace std;



// spark
#include <spark.h>
using namespace Spark;
int main()
{
    printf("hello world\n");

    spark_begin_program();
    {

        auto root = spark_create_control_node(Control::Root);
        spark_push_scope_node(root);
        {
            Int a, b;
            Int c = a + b;

            UInt d = (a + b).As<UInt>();

            Int2 ivec1({12, 27});
            ivec1 + ivec1;
            ivec1 = {13, 61};

            ivec1 = ivec1.XX();
            ivec1.XY = ivec1;

            ivec1[a];

            d = a.As<UInt>();

            a = (d == d);

            Int eq = (c == c);
            UNREFERENCED_PARAMETER(eq);

            Float f = 1.2f;
            -f;

            Float2 fvec1;
            fvec1  = {1.0f, 2.0f};
            ivec1 = (fvec1 == fvec1);

            -ivec1;
        }

        const auto len = spark_node_to_text(root, nullptr, 0);
        unique_ptr<char[]> buff(new char[len]);
        spark_node_to_text(root, buff.get(), len);

        printf("%s\n", buff.get());
    }
    spark_end_program();



    #define PRINT_SIZEOF(TYPE) printf("sizeof(" #TYPE "): %lu\n", sizeof(TYPE))

    PRINT_SIZEOF(Spark::Int);
    PRINT_SIZEOF(Spark::UInt);
    PRINT_SIZEOF(Spark::Int2);

    PRINT_SIZEOF(Spark::Node);
    PRINT_SIZEOF(Spark::rvalue<Spark::Int>);


    #define PRINT_OFFSETOF(TYPE, MEMBER) printf("offsetof(" #TYPE ", " #MEMBER ") : %lu\n", __builtin_offsetof(TYPE, MEMBER))

    PRINT_OFFSETOF(Spark::Node, _children);
    PRINT_OFFSETOF(Spark::Node, _type);

    char prop[16] = {0};
    cout << spark_property_to_str(Property::X, prop, countof(prop)) << endl;
    cout << spark_property_to_str(Property::XX, prop, countof(prop)) << endl;
    cout << spark_property_to_str(Property::WX, prop, countof(prop)) << endl;
    cout << spark_property_to_str(Property::YXWX, prop, countof(prop)) << endl;
    cout << spark_property_to_str(Property::X, prop, countof(prop)) << endl;
    cout << spark_property_to_str(Property::Lo, prop, countof(prop)) << endl;

    return 0;
}

