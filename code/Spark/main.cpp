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

    a = (c == d);

    c == c;

    Float f;
    -f;
    +f;

    Float2 fvec1;
    ivec1 = (fvec1 == fvec1);

    -ivec1;


    #define PRINT_SIZEOF(TYPE) printf("sizeof(" #TYPE "): %lu\n", sizeof(TYPE))

    PRINT_SIZEOF(Spark::Int);
    PRINT_SIZEOF(Spark::UInt);
    PRINT_SIZEOF(Spark::Int2);

    return 0;
}

