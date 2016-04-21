#pragma once

namespace Spark
{
     /// Type Translations

    // is the type an integer (not floating point)
    template<typename T> struct is_integer_type {const static bool value = false;};

    // gets the scalar int type with identical size
    template<size_t S> struct int_by_size {typedef void type;};
#define MAKE_INT_BY_SIZE(TYPE) template<> struct int_by_size<sizeof(TYPE)> {typedef TYPE type;};
    MAKE_INT_BY_SIZE(cl_char)
    MAKE_INT_BY_SIZE(cl_short)
    MAKE_INT_BY_SIZE(cl_int)
    MAKE_INT_BY_SIZE(cl_long)

    /// Wrapper Objects

    template<typename TYPE, unsigned ID>
    struct property_r
    {
        operator TYPE() const
        {
            TRACE
            return TYPE();
        };

        const TYPE operator()() const
        {
            TRACE
            return TYPE();
        }
    };

    template<typename TYPE, unsigned ID>
    struct property_rw
    {
        operator TYPE() const
        {
            TRACE
            return TYPE();
        };

        property_rw& operator=(const TYPE& right)
        {
            UNREFERENCED_PARAMETER(right);
            TRACE
            return *this;
        }
    };

    /// Types

    struct base
    {

    };

    // scalar type wrapper
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
        explicit operator scalar<S>() const
        {
            TRACE;
            return S();
        }

        typedef scalar<cl_int> int_type;
    };

    // wrapper for all vector2 types
    template<typename CL_TYPE>
    struct vector2
    {
        // constructors
        vector2()
        {
            TRACE
        }

        vector2(const CL_TYPE& val)
        {
            UNREFERENCED_PARAMETER(val);
            TRACE
        }

        // properties
        union
        {
            property_rw<scalar<CL_TYPE>, 1u> X;
            property_rw<scalar<CL_TYPE>, 2u> Y;
            property_r<vector2<CL_TYPE>, 4u> XX;
            property_rw<vector2<CL_TYPE>, 5u> XY;
            property_rw<vector2<CL_TYPE>, 7u> YX;
            property_r<vector2<CL_TYPE>, 8u> YY;
        };

        // cast operators
        template<typename S>
        explicit operator vector2<S>() const
        {
            TRACE;
            return vector2<S>();
        }

        typedef vector2<typename int_by_size<sizeof(CL_TYPE)>::type> int_type;
    };

    template<typename T>
    using int_type = typename T::int_type;

    /// Operators

    #define MAKE_UNARY_OPERATOR(RETURN, OP, MUST_BE_INT)\
    template<typename T>\
    RETURN operator OP(const T& right)\
    {\
        static_assert(!MUST_BE_INT || is_integer_type<T>::value, "unary \"" #OP "\" operator only supports integer types");\
        UNREFERENCED_PARAMETER(right);\
        TRACE\
        return RETURN();\
    }

    #define MAKE_BINARY_OPERATOR(RETURN, OP, MUST_BE_INT)\
    template<typename T>\
    RETURN operator OP (const T& left, const T& right)\
    {\
        static_assert(!MUST_BE_INT || is_integer_type<T>::value, "binary \"" #OP "\" operator only supports integer types");\
        UNREFERENCED_PARAMETER(left);\
        UNREFERENCED_PARAMETER(right);\
        TRACE\
        return RETURN();\
    }

    // arithmetic operators
    MAKE_UNARY_OPERATOR(T, +, false)
    MAKE_UNARY_OPERATOR(T, -, false)
    MAKE_BINARY_OPERATOR(T, +, false)
    MAKE_BINARY_OPERATOR(T, -, false)
    MAKE_BINARY_OPERATOR(T, *, false)
    MAKE_BINARY_OPERATOR(T, /, false)
    MAKE_BINARY_OPERATOR(T, %, true)

    // relational operators
    MAKE_BINARY_OPERATOR(int_type<T>, >, false)
    MAKE_BINARY_OPERATOR(int_type<T>, <, false)
    MAKE_BINARY_OPERATOR(int_type<T>, >=, false)
    MAKE_BINARY_OPERATOR(int_type<T>, <=, false)

    // equality operators
    MAKE_BINARY_OPERATOR(int_type<T>, !=, false)
    MAKE_BINARY_OPERATOR(int_type<T>, ==, false)

    // logical operators
    MAKE_UNARY_OPERATOR(int_type<T>, !, false)
    MAKE_BINARY_OPERATOR(int_type<T>, &&, false)
    MAKE_BINARY_OPERATOR(int_type<T>, ||, false)

    // bitwise operators
    MAKE_UNARY_OPERATOR(T, ~, true)
    MAKE_BINARY_OPERATOR(T, &, true)
    MAKE_BINARY_OPERATOR(T, |, true)
    MAKE_BINARY_OPERATOR(T, ^, true)

    // shift operators
    MAKE_BINARY_OPERATOR(T, >>, true)
    MAKE_BINARY_OPERATOR(T, <<, true)

    #define MAKE_INT_TYPE(TYPE, CL_TYPE)\
    typedef scalar<CL_TYPE> TYPE;\
    template<> struct is_integer_type<TYPE> {const static bool value = true;};\
    typedef vector2<CL_TYPE> TYPE##2;\

    #define MAKE_FLOAT_TYPE(TYPE, CL_TYPE)\
    typedef scalar<CL_TYPE> TYPE;\
    typedef vector2<CL_TYPE> TYPE##2;\

    MAKE_INT_TYPE(Char, cl_char)
    MAKE_INT_TYPE(UChar, cl_uchar)
    MAKE_INT_TYPE(Short, cl_short)
    MAKE_INT_TYPE(UShort, cl_ushort)
    MAKE_INT_TYPE(Int, cl_int)
    MAKE_INT_TYPE(UInt, cl_uint)
    MAKE_INT_TYPE(Long, cl_long)
    MAKE_INT_TYPE(ULong, cl_ulong)

    MAKE_FLOAT_TYPE(Float, cl_float)
    MAKE_FLOAT_TYPE(Double, cl_double)
}