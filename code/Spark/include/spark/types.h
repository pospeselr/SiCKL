#pragma once

namespace Spark
{
    using namespace Internal;

    /// int_type used to determine what boolean operators should return

    template<typename T>
    using int_type = typename T::int_type;

    /// Operators

    #define MAKE_UNARY_OPERATOR(RETURN_TYPE, TYPE, OP, ENUM)\
    inline\
    __attribute__((always_inline))\
    const rvalue<RETURN_TYPE> operator OP(const rvalue<TYPE>& right)\
    {\
        const auto dt = static_cast<spark_datatype_t>(RETURN_TYPE::type);\
        const auto op = static_cast<spark_operator_t>(Operator::ENUM);\
        return rvalue<RETURN_TYPE>(spark_create_operator1_node(dt, op, right._node));\
    }

    #define MAKE_BINARY_OPERATOR(RETURN_TYPE, TYPE, OP, ENUM)\
    inline\
    __attribute__((always_inline))\
    const rvalue<RETURN_TYPE> operator OP (const rvalue<TYPE>& left, const rvalue<TYPE>& right)\
    {\
        const auto dt = static_cast<spark_datatype_t>(RETURN_TYPE::type);\
        const auto op = static_cast<spark_operator_t>(Operator::ENUM);\
        return rvalue<RETURN_TYPE>(spark_create_operator2_node(dt, op, left._node, right._node));\
    }

    #define MAKE_PREFIX_OPERATOR(RETURN_TYPE, TYPE, OP, ENUM)\
    inline\
    __attribute__((always_inline))\
    const rvalue<RETURN_TYPE, true> operator OP (const TYPE& value)\
    {\
        const auto dt = static_cast<spark_datatype_t>(RETURN_TYPE::type);\
        const auto op = static_cast<spark_operator_t>(Operator::ENUM);\
        return rvalue<RETURN_TYPE, true>(spark_create_operator1_node(dt, op, value._node));\
    }

    #define MAKE_POSTFIX_OPERATOR(RETURN_TYPE, TYPE, OP, ENUM)\
    inline\
    __attribute__((always_inline))\
    const rvalue<RETURN_TYPE, true> operator OP (const TYPE& value, int)\
    {\
        const auto dt = static_cast<spark_datatype_t>(RETURN_TYPE::type);\
        const auto op = static_cast<spark_operator_t>(Operator::ENUM);\
        return rvalue<RETURN_TYPE, true>(spark_create_operator1_node(dt, op, value._node));\
    }

    #define MAKE_INT_OPERATORS(TYPE)\
    MAKE_UNARY_OPERATOR(TYPE, TYPE, -, Negate)\
    MAKE_BINARY_OPERATOR(TYPE, TYPE, +, Add)\
    MAKE_BINARY_OPERATOR(TYPE, TYPE, -, Subtract)\
    MAKE_BINARY_OPERATOR(TYPE, TYPE, *, Multiply)\
    MAKE_BINARY_OPERATOR(TYPE, TYPE, /, Divide)\
    MAKE_BINARY_OPERATOR(TYPE, TYPE, %, Modulo)\
    MAKE_BINARY_OPERATOR(int_type<TYPE>, TYPE, >, GreaterThan)\
    MAKE_BINARY_OPERATOR(int_type<TYPE>, TYPE, <, LessThan)\
    MAKE_BINARY_OPERATOR(int_type<TYPE>, TYPE, >=, GreaterEqualThan)\
    MAKE_BINARY_OPERATOR(int_type<TYPE>, TYPE, <=, LessEqualThan)\
    MAKE_BINARY_OPERATOR(int_type<TYPE>, TYPE, !=, NotEqual)\
    MAKE_BINARY_OPERATOR(int_type<TYPE>, TYPE, ==, Equal)\
    MAKE_UNARY_OPERATOR(int_type<TYPE>, TYPE, !, LogicalNot)\
    MAKE_BINARY_OPERATOR(int_type<TYPE>, TYPE, &&, LogicalAnd)\
    MAKE_BINARY_OPERATOR(int_type<TYPE>, TYPE, ||, LogicalOr)\
    MAKE_UNARY_OPERATOR(TYPE, TYPE, ~, BitwiseNot)\
    MAKE_BINARY_OPERATOR(TYPE, TYPE, &, BitwiseAnd)\
    MAKE_BINARY_OPERATOR(TYPE, TYPE, |, BitwiseOr)\
    MAKE_BINARY_OPERATOR(TYPE, TYPE, ^, BitwiseXor)\
    MAKE_BINARY_OPERATOR(TYPE, TYPE, >>, RightShift)\
    MAKE_BINARY_OPERATOR(TYPE, TYPE, <<, LeftShift)\
    MAKE_PREFIX_OPERATOR(TYPE, TYPE, ++, PrefixIncrement)\
    MAKE_PREFIX_OPERATOR(TYPE, TYPE, --, PrefixDecrement)\
    MAKE_POSTFIX_OPERATOR(TYPE, TYPE, ++, PostfixIncrement)\
    MAKE_POSTFIX_OPERATOR(TYPE, TYPE, --, PostfixDecrement)\
    MAKE_UNARY_OPERATOR(Pointer<TYPE>, TYPE, &, AddressOf)\

    #define MAKE_FLOAT_OPERATORS(TYPE)\
    MAKE_UNARY_OPERATOR(TYPE, TYPE, -, Negate)\
    MAKE_BINARY_OPERATOR(TYPE, TYPE, +, Add)\
    MAKE_BINARY_OPERATOR(TYPE, TYPE, -, Subtract)\
    MAKE_BINARY_OPERATOR(TYPE, TYPE, *, Multiply)\
    MAKE_BINARY_OPERATOR(TYPE, TYPE, /, Divide)\
    MAKE_BINARY_OPERATOR(int_type<TYPE>, TYPE, >, GreaterThan)\
    MAKE_BINARY_OPERATOR(int_type<TYPE>, TYPE, <, LessThan)\
    MAKE_BINARY_OPERATOR(int_type<TYPE>, TYPE, >=, GreaterEqualThan)\
    MAKE_BINARY_OPERATOR(int_type<TYPE>, TYPE, <=, LessEqualThan)\
    MAKE_BINARY_OPERATOR(int_type<TYPE>, TYPE, !=, NotEqual)\
    MAKE_BINARY_OPERATOR(int_type<TYPE>, TYPE, ==, Equal)\
    MAKE_UNARY_OPERATOR(int_type<TYPE>, TYPE, !, LogicalNot)\
    MAKE_BINARY_OPERATOR(int_type<TYPE>, TYPE, &&, LogicalAnd)\
    MAKE_BINARY_OPERATOR(int_type<TYPE>, TYPE, ||, LogicalOr)\
    MAKE_UNARY_OPERATOR(Pointer<TYPE>, TYPE, &, AddressOf)\

    #define MAKE_TYPEDEFS(TYPE, RAW_TYPE)\
    typedef scalar<RAW_TYPE> TYPE;\
    typedef vector2<scalar<RAW_TYPE>> TYPE##2;\
    typedef Pointer<TYPE> P##TYPE;\
    typedef Pointer<TYPE##2> P##TYPE##2;\

    #define MAKE_INT_TYPES(TYPE, CL_TYPE)\
    MAKE_TYPEDEFS(TYPE, CL_TYPE)\
    MAKE_INT_OPERATORS(TYPE)\
    MAKE_INT_OPERATORS(TYPE##2)\

    #define MAKE_FLOAT_TYPES(TYPE, CL_TYPE)\
    MAKE_TYPEDEFS(TYPE, CL_TYPE)\
    MAKE_FLOAT_OPERATORS(TYPE)\
    MAKE_FLOAT_OPERATORS(TYPE##2)\

    // Int has to appear first since it is a return type of other scalar comparison operators
    MAKE_INT_TYPES(Int, int32_t);
    // other ints
    MAKE_INT_TYPES(Char, int8_t);
    MAKE_INT_TYPES(UChar, uint8_t);
    MAKE_INT_TYPES(Short, int16_t);
    MAKE_INT_TYPES(UShort, uint16_t);
    MAKE_INT_TYPES(UInt, uint32_t);
    MAKE_INT_TYPES(Long, int64_t);
    MAKE_INT_TYPES(ULong, uint64_t);

    // floating point
    MAKE_FLOAT_TYPES(Float, float);
    MAKE_FLOAT_TYPES(Double, double);

    template<typename TYPE>
    inline
    __attribute__((always_inline))
    const rvalue<Pointer<TYPE>> operator+(const Pointer<TYPE>& pointer, const rvalue<UInt>& offset)
    {
        const auto dt = static_cast<spark_datatype_t>(Datatype(TYPE::type.GetPrimitive(), TYPE::type.GetComponents(), true));
        const auto op = static_cast<spark_operator_t>(Operator::Add);
        return rvalue<Pointer<TYPE>>(spark_create_operator2_node(dt, op, pointer._node, offset._node));
    }

    template<typename TYPE>
    inline
    __attribute__((always_inline))
    const rvalue<Pointer<TYPE>> operator+(const rvalue<UInt>& offset, const Pointer<TYPE>& pointer)
    {
        const auto dt = static_cast<spark_datatype_t>(Datatype(TYPE::type.GetPrimitive(), TYPE::type.GetComponents(), true));
        const auto op = static_cast<spark_operator_t>(Operator::Add);
        return rvalue<Pointer<TYPE>>(spark_create_operator2_node(dt, op, offset._node, pointer._node));
    }

}