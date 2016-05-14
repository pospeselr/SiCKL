#include "precomp.h"

#include <cstdio>
#include <memory>
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
            a + b;
            a = 1;

            UInt c, d;
            c + d;

            Int2 ivec1;
            ivec1 + ivec1;

            ivec1 = ivec1.XX().XX();

            ivec1[a];

            a = (Int)c;

            a = (c == d);

            c == c;

            Float f = 1.2f;
            -f;

            Float2 fvec1 = 12.0f;
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

    #define PRINT_OFFSETOF(TYPE, MEMBER) printf("offsetof(" #TYPE ", " #MEMBER ") : %lu\n", __builtin_offsetof(TYPE, MEMBER))

    PRINT_OFFSETOF(Spark::Node, _children);
    PRINT_OFFSETOF(Spark::Node, _type);


    return 0;
}

