#include "precomp.h"

#include <cstdio>
using namespace std;

// spark
#include <spark.h>
using namespace Spark;
int main()
{
    printf("hello world\n");

    auto root = spark_create_control_node(Control::Root);
    spark_push_scope_node(root);

    Int a, b;
    a + b;
    UInt c, d;
    c + d;

    Int2 ivec1;
    ivec1 + ivec1;

    ivec1 = ivec1.XX().XX;

    ivec1[a];

    a = (Int)c;

    a = (c == d);

    c == c;

    Float f;
    -f;

    Float2 fvec1;
    ivec1 = (fvec1 == fvec1);

    -ivec1;


    #define PRINT_SIZEOF(TYPE) printf("sizeof(" #TYPE "): %lu\n", sizeof(TYPE))

    PRINT_SIZEOF(Spark::Int);
    PRINT_SIZEOF(Spark::UInt);
    PRINT_SIZEOF(Spark::Int2);

    PRINT_SIZEOF(Spark::Node);

    #define PRINT_OFFSETOF(TYPE, MEMBER) printf("offsetof(" #TYPE ", " #MEMBER ") : %lu\n", __builtin_offsetof(TYPE, MEMBER))

    PRINT_OFFSETOF(Spark::Node, _children);
    PRINT_OFFSETOF(Spark::Node, _next);
    PRINT_OFFSETOF(Spark::Node, _nodeType);


    return 0;
}

