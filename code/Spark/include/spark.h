#include "common.h"
#include "enums.h"

namespace Spark
{
    /// Type Translations

    // Contains the type returned by comparison operators
    template<typename T> struct opencl_type {typedef void type; };

    template<typename T> struct is_integer_type {const static bool value = false;};

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

        typedef scalar<cl_int> bool_type;
    };


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

        // properties
        union
        {
            property_rw<TYPE, 1u> X;
            property_rw<TYPE, 2u> Y;
            property_r<vector2<TYPE>, 4u> XX;
            property_rw<vector2<TYPE>, 5u> XY;
            property_rw<vector2<TYPE>, 7u> YX;
            property_r<vector2<TYPE>, 8u> YY;
        };

        // cast operators
        template<typename S>
        explicit operator vector2<S>() const
        {
            TRACE;
            return vector2<S>();
        }

        typedef vector2<typename TYPE::bool_type> bool_type;
    };

    template<typename T>
    using bool_type = typename T::bool_type;

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
    MAKE_BINARY_OPERATOR(bool_type<T>, >, false)
    MAKE_BINARY_OPERATOR(bool_type<T>, <, false)
    MAKE_BINARY_OPERATOR(bool_type<T>, >=, false)
    MAKE_BINARY_OPERATOR(bool_type<T>, <=, false)

    // equality operators
    MAKE_BINARY_OPERATOR(bool_type<T>, !=, false)
    MAKE_BINARY_OPERATOR(bool_type<T>, ==, false)

    // bitwise operators
    MAKE_UNARY_OPERATOR(T, ~, true)
    MAKE_BINARY_OPERATOR(T, &, true)
    MAKE_BINARY_OPERATOR(T, |, true)
    MAKE_BINARY_OPERATOR(T, ^, true)


    typedef scalar<cl_int> Int;
    template<> struct is_integer_type<Int> {const static bool value = true;};

    typedef scalar<cl_uint> UInt;
    template<> struct is_integer_type<UInt> {const static bool value = true;};

    typedef scalar<cl_float> Float;


    typedef vector2<Int> Int2;
    typedef vector2<UInt> UInt2;
}

