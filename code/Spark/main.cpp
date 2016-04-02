#include <cstdio>
using namespace std;

// opencl
#ifdef __APPLE__
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

// spark
#include <spark.h>
using namespace Spark;
int main()
{
    printf("hello world\n");

    Int a, b;
    a + b;
    UInt c, d;
    c + d;

    Int2 ivec1;
    ivec1 + ivec1;

    a = (Int)c;

    c == c;

    Float f;
    -f;
    +f;

    -ivec1;

    //f & f;


#if 0


    Int result = (a == b);
    result = (c == c);
    result = c != c;

    a + b;
    a % b;

    (Int)c;
    (UInt)a;
    (Float)a;


    Float f = 1.0f;
    Double d;

    f = f - f;

    f = (Float)d;
    Int2 ivec1;
    ivec1 + ivec1;

    Float2 fvec1;
    fvec1 + fvec1;
    //Int2 ivec2 = 1;

    Int2 ivec2 = (Int2)fvec1;
    ivec2 + ivec1;
    ivec2 = ivec2.XX;
    a = ivec2.XX().X;
    ivec2.XY = ivec2.XY;

#endif
    #define PRINT_SIZEOF(TYPE) printf("sizeof(" #TYPE "): %lu\n", sizeof(TYPE))

    PRINT_SIZEOF(Spark::Int);
    PRINT_SIZEOF(Spark::UInt);
    PRINT_SIZEOF(Spark::Int2);
}

