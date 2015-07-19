/*
 * macro definition/declerations for supported built-in functions
 */

namespace SiCKL
{
    
    FUNC_0(UInt3, Index)
    FUNC_0(Float3, NormalizedIndex)
    // trig functions
    FUNC_1(Float, Sin, Float)
    FUNC_1(Float, Cos, Float)
    FUNC_1(Float, Tan, Float)
    FUNC_1(Float, ASin, Float)
    FUNC_1(Float, ACos, Float)
    FUNC_1(Float, ATan, Float)
    FUNC_1(Float, SinH, Float)
    FUNC_1(Float, CosH, Float)
    FUNC_1(Float, TanH, Float)
    
    // exponential functions
    FUNC_2(Float, Pow, Float, Float)
    FUNC_1(Float, Exp, Float)
    FUNC_1(Float, Log, Float)
    FUNC_1(Float, Exp2, Float)
    FUNC_1(Float, Log2, Float)
    FUNC_1(Float, Sqrt, Float)
    // common
    FUNC_1(Float, Abs, Float)
    FUNC_1(Int, Abs, Int)
    FUNC_1(Float, Sign, Float)
    FUNC_1(Int, Sign, Int)
    FUNC_1(Float, Floor, Float)
    FUNC_1(Float, Ceiling, Float)
    FUNC_2(Float, Min, Float, Float)
    FUNC_2(Float, Max, Float, Float)
    FUNC_3(Float, Clamp, Float, Float, Float)
    FUNC_1(Float, IsNan, Float)
    FUNC_1(Float, IsInf, Float)
    // vector math
    FUNC_1(Float, Length, Float2)
    FUNC_2(Float, Distance, Float2, Float2)
    FUNC_2(Float, Dot, Float2, Float2)
}
