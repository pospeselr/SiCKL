#include "common.h"
#include "enums.h"

namespace Spark
{
    #define MAKE_UNARY_OPERATOR(RETURN, OP)\
    RETURN operator OP () const\
    {\
        TRACE\
        return RETURN();\
    }

    #define MAKE_BINARY_OPERATOR(RETURN, OP, TYPE)\
    RETURN operator OP (const TYPE& right) const\
    {\
        UNREFERENCED_PARAMETER(right)\
        TRACE\
        return RETURN();\
    }

    /// Type Translations

    // Contains the type returned by comparison operators
    template<typename T> struct bool_type { typedef void type_t; };
    template<typename T> struct opencl_type {typedef void type_t; };

    /// Type Building  Blocks

    // Vector types
    template<typename T> struct integer_scalar;
    template<typename T> struct float_scalar;

    template<typename CL_TYPE>
    struct scalar
    {
        // constructors

        scalar()
        {
            TRACE
        }

        scalar(const CL_TYPE val)
        {
            UNREFERENCED_PARAMETER(val);
            TRACE
        }

        // cast operators

        template<typename S>
        explicit operator integer_scalar<S>() const;

        template<typename S>
        explicit operator float_scalar<S>() const;
    };

    template<typename T> struct integer_vector2;
    template<typename T> struct float_vector2;

    template<typename TYPE>
    struct vector2
    {
        // constructors

        vector2()
        {
            TRACE
        }

        vector2(const TYPE& val)
        {
            UNREFERENCED_PARAMETER(val);
            TRACE
        }

        // cast operators
        template<typename S>
        explicit operator integer_vector2<S>() const;

        template<typename S>
        explicit operator float_vector2<S>() const;
    };

    // Operations

    template<typename TYPE>
    struct arithmetic_operand
    {
        MAKE_BINARY_OPERATOR(TYPE, +, TYPE)
        MAKE_BINARY_OPERATOR(TYPE, -, TYPE)
        MAKE_BINARY_OPERATOR(TYPE, *, TYPE)
        MAKE_BINARY_OPERATOR(TYPE, /, TYPE)
        MAKE_UNARY_OPERATOR(TYPE, +)
        MAKE_UNARY_OPERATOR(TYPE, -)
    };

    template<typename TYPE>
    struct remainder_operand
    {
        MAKE_BINARY_OPERATOR(TYPE, %, TYPE)
    };

    template<typename TYPE>
    struct equality_operand
    {
        using BOOL_TYPE = typename bool_type<TYPE>::type_t;
        MAKE_BINARY_OPERATOR(BOOL_TYPE, !=, TYPE)
        MAKE_BINARY_OPERATOR(BOOL_TYPE, ==, TYPE)
    };

    template<typename TYPE>
    struct relational_operand
    {
        using BOOL_TYPE = typename bool_type<TYPE>::type_t;
        MAKE_BINARY_OPERATOR(BOOL_TYPE, >, TYPE)
        MAKE_BINARY_OPERATOR(BOOL_TYPE, <, TYPE)
        MAKE_BINARY_OPERATOR(BOOL_TYPE, >=, TYPE)
        MAKE_BINARY_OPERATOR(BOOL_TYPE, <=, TYPE)
    };

    template<typename TYPE>
    struct bitwise_operand
    {
        MAKE_BINARY_OPERATOR(TYPE, &, TYPE)
        MAKE_BINARY_OPERATOR(TYPE, |, TYPE)
        MAKE_BINARY_OPERATOR(TYPE, ^, TYPE)
        MAKE_UNARY_OPERATOR(TYPE, ~)
    };

    template<typename TYPE>
    struct logical_operand
    {
        using BOOL_TYPE = typename bool_type<TYPE>::type_t;
        MAKE_BINARY_OPERATOR(BOOL_TYPE, &&, TYPE)
        MAKE_BINARY_OPERATOR(BOOL_TYPE, ||, TYPE)
        MAKE_UNARY_OPERATOR(BOOL_TYPE, !)
    };

    template<typename TYPE>
    struct bitshift_operand
    {
        MAKE_BINARY_OPERATOR(TYPE, >>, TYPE)
        MAKE_BINARY_OPERATOR(TYPE, <<, TYPE)
    };


    /// Template type for each primitives

    // Scalar Types

    template<typename OPENCL_TYPE>
    struct integer_scalar :
        public scalar<OPENCL_TYPE>,
        public arithmetic_operand<integer_scalar<OPENCL_TYPE>>,
        public remainder_operand<integer_scalar<OPENCL_TYPE>>,
        public equality_operand<integer_scalar<OPENCL_TYPE>>,
        public relational_operand<integer_scalar<OPENCL_TYPE>>,
        public logical_operand<integer_scalar<OPENCL_TYPE>>,
        public bitshift_operand<integer_scalar<OPENCL_TYPE>>
    {
        using scalar<OPENCL_TYPE>::scalar;
    };

    template<typename FROM> template<typename TO>
    scalar<FROM>::operator integer_scalar<TO>() const
    {
        TRACE
        return integer_scalar<TO>();
    }

    template<typename OPENCL_TYPE>
    struct float_scalar :
        public scalar<OPENCL_TYPE>,
        public arithmetic_operand<float_scalar<OPENCL_TYPE>>,
        public equality_operand<float_scalar<OPENCL_TYPE>>,
        public relational_operand<float_scalar<OPENCL_TYPE>>
    {
        using scalar<OPENCL_TYPE>::scalar;
    };

    template<typename FROM> template<typename TO>
    scalar<FROM>::operator float_scalar<TO>() const
    {
        TRACE
        return float_scalar<TO>();
    }

    // Vector2 Types

    template<typename TYPE>
    struct integer_vector2 :
        public vector2<TYPE>,
        public arithmetic_operand<integer_vector2<TYPE>>,
        public remainder_operand<integer_vector2<TYPE>>,
        public equality_operand<integer_vector2<TYPE>>,
        public relational_operand<integer_vector2<TYPE>>,
        public logical_operand<integer_vector2<TYPE>>,
        public bitshift_operand<integer_vector2<TYPE>>
    {
        using vector2<TYPE>::vector2;
    };

    template<typename FROM> template<typename TO>
    vector2<FROM>::operator integer_vector2<TO>() const
    {
        TRACE
        return integer_vector2<TO>();
    }

    template<typename TYPE>
    struct float_vector2 :
        public vector2<TYPE>,
        public arithmetic_operand<float_vector2<TYPE>>,
        public equality_operand<float_vector2<TYPE>>,
        public relational_operand<float_vector2<TYPE>>
    {
        using vector2<TYPE>::vector2;
    };

    template<typename FROM> template<typename TO>
    vector2<FROM>::operator float_vector2<TO>() const
    {
        TRACE
        return float_vector2<TO>();
    }

    /// Macros

    // type conversions
    #define MAKE_BOOL_TYPE(TYPE, BOOL_TYPE) template<> struct bool_type<TYPE> {typedef BOOL_TYPE type_t;};
    #define MAKE_OPENCL_TYPE(TYPE, OPENCL_TYPE) template<> struct opencl_type<TYPE> {typedef OPENCL_TYPE type_t;};

    // type generation
    #define MAKE_FLOAT_TYPES(TYPE, OPENCL_TYPE)\
    typedef float_scalar<OPENCL_TYPE> TYPE;\
    typedef float_vector2<TYPE> TYPE##2;\
    MAKE_BOOL_TYPE(TYPE, Int)\
    MAKE_BOOL_TYPE(TYPE##2, Int2)\
    MAKE_OPENCL_TYPE(TYPE, OPENCL_TYPE)\
    MAKE_OPENCL_TYPE(TYPE##2, OPENCL_TYPE##2)

    #define MAKE_INTEGER_TYPES(TYPE, OPENCL_TYPE)\
    typedef integer_scalar<OPENCL_TYPE> TYPE;\
    typedef integer_vector2<TYPE> TYPE##2;\
    MAKE_BOOL_TYPE(TYPE, Int)\
    MAKE_BOOL_TYPE(TYPE##2, Int2)\
    MAKE_OPENCL_TYPE(TYPE, OPENCL_TYPE)\
    MAKE_OPENCL_TYPE(TYPE##2, OPENCL_TYPE##2)

    /// Typedefs

    // Integer Scalars
    MAKE_INTEGER_TYPES(Int, cl_int)
    MAKE_INTEGER_TYPES(UInt, cl_uint)
    MAKE_INTEGER_TYPES(Char, cl_char)
    MAKE_INTEGER_TYPES(UChar, cl_uchar)
    MAKE_INTEGER_TYPES(Short, cl_short)
    MAKE_INTEGER_TYPES(UShort, cl_ushort)
    MAKE_INTEGER_TYPES(Long, cl_long)
    MAKE_INTEGER_TYPES(ULong, cl_ulong)

    // Floating-point Scalars
    MAKE_FLOAT_TYPES(Float, cl_float)
    MAKE_FLOAT_TYPES(Double, cl_double)
}

