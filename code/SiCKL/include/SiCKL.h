#pragma once

enum
{
    Unknown,
    MSVC,
    Clang,
#ifdef _MSC_VER
    Compiler = MSVC
#endif

#ifdef __clang__
    Compiler = Clang
#endif

};

// C
#include <stdio.h>
#include <stdint.h>
#include <string.h>

// C++
#include <set>
#include <vector>


#if (Compiler == MSVC)
    #include <intrin.h>
    #define DEBUGBREAK() __debugbreak()
#elif (Compiler == Clang)
    #define DEBUGBREAK() __builtin_trap()
#endif

#define SICKL_ASSERT(X) if(!(X)) {printf("Assert Failed: %s line %i on \"%s\"\n", __FILE__, __LINE__, #X);  DEBUGBREAK();}

#ifdef __APPLE__
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

// SiCKL headers
#include "Common.h"

// These headers contain all the logic for writing SiCKL Kernels
#include "Enums.h"
#include "AST.h"
#include "Source.h"
#include "Declares.h"
#include "KernelTypes.h"
#include "KernelFunctions.h"
#include "KernelBuffers.h"
#include "KernelConstructs.h"

// These headers contain the public SiCKL APIs
#include "Runtime.h"
#include "DeviceBuffer.h"
#include "HostBuffer.h"
#include "Program.h"
#include "Compiler.h"
