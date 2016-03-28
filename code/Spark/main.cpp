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
    UInt c;
    Int result = (a == b);
    result = (c == c);
    result = c != c;

    a + b;
    a % b;

    (Int)c;
    (UInt)a;
    (Float)a;

    #define PRINT_SIZEOF(TYPE) printf("sizeof(" #TYPE "): %lu\n", sizeof(TYPE))

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


    PRINT_SIZEOF(Spark::Int);
    PRINT_SIZEOF(Spark::UInt);
    PRINT_SIZEOF(Spark::Int2);
}

