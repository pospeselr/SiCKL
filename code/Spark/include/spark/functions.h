#pragma once

namespace spark
{
    // built-ins

    const Int2 Index()
    {
        const auto dt = static_cast<spark_datatype_t>(Int::type);
        const auto op = static_cast<spark_operator_t>(spark::shared::Operator::Index);
        return client::rvalue<Int2>(spark_create_operator_node(dt, op, THROW_ON_ERROR()));
    }

    const Float2 NormalizedIndex()
    {
        const auto dt = static_cast<spark_datatype_t>(Int::type);
        const auto op = static_cast<spark_operator_t>(spark::shared::Operator::NormalizedIndex);
        return client::rvalue<Float2>(spark_create_operator_node(dt, op, THROW_ON_ERROR()));
    }

    #define MAKE_FUNCTION_1(RETURN_TYPE, TYPE, NAME, ENUM)\
    SPARK_FORCE_INLINE\
    const client::rvalue<RETURN_TYPE> NAME(const client::rvalue<TYPE>& arg1)\
    {\
        const auto dt = static_cast<spark_datatype_t>(RETURN_TYPE::type);\
        const auto op = static_cast<spark_operator_t>(spark::shared::Operator::ENUM);\
        return client::rvalue<RETURN_TYPE>(spark_create_operator1_node(dt, op, arg1._node));\
    }

    #define MAKE_FUNCTION_2(RETURN_TYPE, TYPE1, TYPE2, NAME, ENUM)\
    SPARK_FORCE_INLINE\
    const client::rvalue<RETURN_TYPE> NAME(const client::rvalue<TYPE1>& arg1, const client::rvalue<TYPE2>& arg2)\
    {\
        const auto dt = static_cast<spark_datatype_t>(RETURN_TYPE::type);\
        const auto op = static_cast<spark_operator_t>(spark::shared::Operator::ENUM);\
        return client::rvalue<RETURN_TYPE>(spark_create_operator2_node(dt, op, arg1._node, arg2._node));\
    }

    #define MAKE_FUNCTION_3(RETURN_TYPE, TYPE1, TYPE2, TYPE3, NAME, ENUM)\
    SPARK_FORCE_INLINE\
    const client::rvalue<RETURN_TYPE> NAME(const client::rvalue<TYPE1>& arg1, const client::rvalue<TYPE2>& arg2, const client::rvalue<TYPE3>& arg3)\
    {\
        const auto dt = static_cast<spark_datatype_t>(RETURN_TYPE::type);\
        const auto op = static_cast<spark_operator_t>(spark::shared::Operator::ENUM);\
        return client::rvalue<RETURN_TYPE>(spark_create_operator3_node(dt, op, arg1._node, arg2._node, arg3._node));\
    }

    #define MAKE_MATH_FUNCTIONS2(TYPE)\
    MAKE_FUNCTION_1(TYPE, TYPE, ArcCos, ArcCos)\
    MAKE_FUNCTION_1(TYPE, TYPE, ArcCosh, ArcCosh)\
    MAKE_FUNCTION_1(TYPE, TYPE, ArcSin, ArcSin)\
    MAKE_FUNCTION_1(TYPE, TYPE, ArcSinh, ArcSinh)\
    MAKE_FUNCTION_1(TYPE, TYPE, ArcTan, ArcTan)\
    MAKE_FUNCTION_2(TYPE, TYPE, TYPE, ArcTan2, ArcTan2)\
    MAKE_FUNCTION_1(TYPE, TYPE, ArcTanh, ArcTanh)\
    MAKE_FUNCTION_1(TYPE, TYPE, CubeRoot, CubeRoot)\
    MAKE_FUNCTION_1(TYPE, TYPE, Ceiling, Ceiling)\
    MAKE_FUNCTION_1(TYPE, TYPE, Cos, Cos)\
    MAKE_FUNCTION_1(TYPE, TYPE, Exp, Exp)\
    MAKE_FUNCTION_1(TYPE, TYPE, Exp2, Exp2)\
    MAKE_FUNCTION_1(TYPE, TYPE, Exp10, Exp10)\
    MAKE_FUNCTION_1(TYPE, TYPE, Abs, FloatAbs)\
    MAKE_FUNCTION_1(TYPE, TYPE, Floor, Floor)\
    MAKE_FUNCTION_3(TYPE, TYPE, TYPE, TYPE, MultiplyAdd, MultiplyAdd)\
    MAKE_FUNCTION_2(TYPE, TYPE, TYPE, Max, FloatMax)\
    MAKE_FUNCTION_2(TYPE, TYPE, TYPE, Min, FloatMin)\
    MAKE_FUNCTION_2(TYPE, TYPE, TYPE, FloatModulus, FloatModulus)\
    MAKE_FUNCTION_2(TYPE, TYPE, Pointer<TYPE>, Fraction, Fraction)\
    MAKE_FUNCTION_2(TYPE, TYPE, TYPE, Hypotenuse, Hypotenuse)\
    MAKE_FUNCTION_1(TYPE, TYPE, LogGamma, LogGamma)\
    MAKE_FUNCTION_1(TYPE, TYPE, Log, Log)\
    MAKE_FUNCTION_1(TYPE, TYPE, Log2, Log2)\
    MAKE_FUNCTION_1(TYPE, TYPE, Log10, Log10)\
    MAKE_FUNCTION_1(TYPE, TYPE, Log1Plus, Log1Plus)\
    MAKE_FUNCTION_3(TYPE, TYPE, TYPE, TYPE, FastMultiplyAdd, FastMultiplyAdd)\
    MAKE_FUNCTION_2(TYPE, TYPE, TYPE, Pow, Pow)\
    MAKE_FUNCTION_2(TYPE, TYPE, TYPE, Remainder, Remainder)\
    MAKE_FUNCTION_1(TYPE, TYPE, InverseSquareRoot, InverseSquareRoot)\
    MAKE_FUNCTION_1(TYPE, TYPE, Sin, Sin)\
    MAKE_FUNCTION_1(TYPE, TYPE, Sinh, Sinh)\
    MAKE_FUNCTION_1(TYPE, TYPE, SquareRoot, SquareRoot)\
    MAKE_FUNCTION_1(TYPE, TYPE, Tan, Tan)\
    MAKE_FUNCTION_1(TYPE, TYPE, Tanh, Tanh)\
    MAKE_FUNCTION_1(TYPE, TYPE, Gamma, Gamma)\
    MAKE_FUNCTION_1(TYPE, TYPE, Truncate, Truncate)\

    #define MAKE_MATH_FUNCTIONS(SCALAR)\
    MAKE_MATH_FUNCTIONS2(SCALAR)\
    MAKE_MATH_FUNCTIONS2(SCALAR##2)\
    MAKE_MATH_FUNCTIONS2(SCALAR##4)\

    #define MAKE_GEOMETRIC_FUNCTIONS2(SCALAR, TYPE)\
    MAKE_FUNCTION_2(SCALAR, TYPE, TYPE, Dot, DotProduct)\
    MAKE_FUNCTION_2(SCALAR, TYPE, TYPE, Distance, Distance)\
    MAKE_FUNCTION_1(SCALAR, TYPE, Length, Length)\
    MAKE_FUNCTION_1(TYPE, TYPE, Normalize, Normalize)\
    MAKE_FUNCTION_2(SCALAR, TYPE, TYPE, FastDistance, FastDistance)\
    MAKE_FUNCTION_1(SCALAR, TYPE, FastLength, FastLength)\
    MAKE_FUNCTION_1(TYPE, TYPE, FastNormalize, FastNormalize)\

    #define MAKE_GEOMETRIC_FUNCTIONS(SCALAR)\
    MAKE_FUNCTION_2(SCALAR##4, SCALAR##4, SCALAR##4, Cross, CrossProduct)\
    MAKE_GEOMETRIC_FUNCTIONS2(SCALAR, SCALAR)\
    MAKE_GEOMETRIC_FUNCTIONS2(SCALAR, SCALAR##2)\
    MAKE_GEOMETRIC_FUNCTIONS2(SCALAR, SCALAR##4)\

    #define MAKE_INTEGER_FUNCTIONS2(UNSIGNED, TYPE)\
    MAKE_FUNCTION_1(UNSIGNED, TYPE, Abs, IntegerAbs)\
    MAKE_FUNCTION_2(UNSIGNED, TYPE, TYPE, AbsDiff, AbsDiff)\
    MAKE_FUNCTION_2(TYPE, TYPE, TYPE, Max, IntegerMax)\
    MAKE_FUNCTION_2(TYPE, TYPE, TYPE, Min, IntegerMin)\

    #define MAKE_INTEGER_FUNCTIONS(SIGNED)\
    MAKE_INTEGER_FUNCTIONS2(U##SIGNED, SIGNED)\
    MAKE_INTEGER_FUNCTIONS2(U##SIGNED, U##SIGNED)\
    MAKE_INTEGER_FUNCTIONS2(U##SIGNED##2, SIGNED##2)\
    MAKE_INTEGER_FUNCTIONS2(U##SIGNED##2, U##SIGNED##2)\
    MAKE_INTEGER_FUNCTIONS2(U##SIGNED##4, SIGNED##4)\
    MAKE_INTEGER_FUNCTIONS2(U##SIGNED##4, U##SIGNED##4)\

    #define MAKE_COMMON_FUNCTIONS2(SCALAR, TYPE)\
    MAKE_FUNCTION_3(TYPE, TYPE, SCALAR, SCALAR, Clamp, Clamp)\
    MAKE_FUNCTION_1(TYPE, TYPE, Degrees, Degrees)\
    MAKE_FUNCTION_1(TYPE, TYPE, Radians, Radians)\
    MAKE_FUNCTION_3(TYPE, TYPE, TYPE, TYPE, Mix, Mix)\
    MAKE_FUNCTION_3(TYPE, TYPE, TYPE, Float, Mix, Mix)\
    MAKE_FUNCTION_2(TYPE, SCALAR, TYPE, Step, Step)\
    MAKE_FUNCTION_3(TYPE, TYPE, TYPE, TYPE, SmoothStep, SmoothStep)\
    MAKE_FUNCTION_3(TYPE, SCALAR, SCALAR, TYPE, SmoothStep, SmoothStep)\
    MAKE_FUNCTION_1(TYPE, TYPE, Sign, Sign)\

    #define MAKE_COMMON_FUNCTIONS(SCALAR)\
    MAKE_FUNCTION_3(SCALAR, SCALAR, SCALAR, SCALAR, Clamp, Clamp)\
    MAKE_FUNCTION_3(SCALAR, SCALAR, SCALAR, Float, Mix, Mix)\
    MAKE_FUNCTION_3(SCALAR, SCALAR, SCALAR, SCALAR, SmoothStep, SmoothStep)\
    MAKE_FUNCTION_1(SCALAR, SCALAR, Sign, Sign)\
    MAKE_COMMON_FUNCTIONS2(SCALAR, SCALAR##2)\
    MAKE_COMMON_FUNCTIONS2(SCALAR, SCALAR##4)\

    MAKE_MATH_FUNCTIONS(Float);
    MAKE_MATH_FUNCTIONS(Double);

    MAKE_GEOMETRIC_FUNCTIONS(Float);
    MAKE_GEOMETRIC_FUNCTIONS(Double);

    MAKE_INTEGER_FUNCTIONS(Char);
    MAKE_INTEGER_FUNCTIONS(Short);
    MAKE_INTEGER_FUNCTIONS(Int);
    MAKE_INTEGER_FUNCTIONS(Long);

    MAKE_COMMON_FUNCTIONS(Float);
    MAKE_COMMON_FUNCTIONS(Double);
}