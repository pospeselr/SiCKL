#pragma once

/*
 * Internal enum definitions
 */

namespace SiCKL
{
    // provides an immutable typesafe typedef wrapper around a primitive type
    #define SAFE_TYPEDEF(NAME, TYPE) \
    struct NAME \
    { \
        NAME() : _val(-1) { } \
        explicit NAME(TYPE in_val) : _val(in_val) { } \
        NAME(const NAME& that) : _val(that._val) { } \
        operator const TYPE&() const { return _val; } \
        NAME& operator=(const TYPE& other) {_val = other; return *this;} \
    private: \
        TYPE _val; \
    }
    
    //typedef int32_t symbol_id_t;
    SAFE_TYPEDEF(symbol_id_t, int32_t);

    typedef uint32_t member_id_t;
    const symbol_id_t invalid_symbol(-1);
    const symbol_id_t temp_symbol(-2);
    const symbol_id_t member_symbol(-3);
    const symbol_id_t lvalue_symbol(-4);
    const symbol_id_t rvalue_symbol(-5);

    struct NodeType
    {
        enum Type
        {
            Invalid = -1,
            // our Root node
            Program,
            // Function related 
            Main,
            Function,
            Parameters,
            Body,
            Return,
            // Flow Control
            If,
            ElseIf,
            Else,
            While,
            ForInRange,
            // Variable Declaration
            OutVar,
            ConstVar,
            Var,
            Literal,
            // Operators
            Assignment,
            // Comparison
            Equal,
            NotEqual,
            Greater,
            GreaterEqual,
            Less,
            LessEqual,
            // Boolean
            LogicalAnd,
            LogicalOr,
            LogicalNot,
            // Bitwise
            BitwiseAnd,
            BitwiseOr,
            BitwiseXor,
            BitwiseNot,
            LeftShift,
            RightShift,

            // Arithmetic
            UnaryMinus,
            Add,
            Subtract,
            Multiply,
            Divide,
            Modulo,

            // Functions calls
            Constructor,
            Cast,
            BuiltinFunction,
            CallUserFunction,
            
            Sample1D,// sample from 1D buffer
            Sample2D,// sample from 2d buffer
            Member,// member variable access

            Max
        };
    };
    typedef NodeType::Type NodeType_t;
    
    // each type is denoted by a bit
    struct DataType
    {
        enum Type
        {
#           define SHIFT(X) (1 << X)
            Invalid = -1,
            Void = SHIFT(0),
            Bool = SHIFT(1),
            Int = SHIFT(2),
            UInt = SHIFT(3),
            Float = SHIFT(4),
            Int2 = SHIFT(5),
            UInt2 = SHIFT(6),
            Float2 = SHIFT(7),
            Int3 = SHIFT(8),
            UInt3 = SHIFT(9),
            Float3 = SHIFT(10),
            Int4 = SHIFT(11),
            UInt4 = SHIFT(12),
            Float4 = SHIFT(13),

            // Buffer types
            Buffer1D = SHIFT(30),
            Buffer2D = SHIFT(31),
#           undef SHIFT
        };
    };
    typedef DataType::Type DataType_t;

    struct BuiltinFunction
    {
        enum Func
        {
            Invalid = -1,
            // info
            Index,
            NormalizedIndex,
            // trigonometry
            Sin,
            Cos,
            Tan,
            ASin,
            ACos,
            ATan,
            SinH,
            CosH,
            TanH,
            ASinH,
            ACosH,
            ATanH,
            // exponential functions
            Pow,
            Exp,
            Log,
            Exp2,
            Log2,
            Sqrt,
            // common
            Abs,
            Sign,
            Floor,
            Ceiling,
            Min,
            Max,
            Clamp,
            IsNan,
            IsInf,
            // vector math
            Length,
            Distance,
            Dot,
            Cross,
            Normalize,
            // buffer methods
            BufferLength,
            BufferWidth,
            BufferHeight,
        };
    };
}



