#pragma once

namespace Spark
{
     /// Type Translations

    // gets the scalar int type with identical size
    template<size_t S> struct int_by_size {typedef void type;};
#define MAKE_INT_BY_SIZE(TYPE) template<> struct int_by_size<sizeof(TYPE)> {typedef TYPE type;};
    MAKE_INT_BY_SIZE(cl_char)
    MAKE_INT_BY_SIZE(cl_short)
    MAKE_INT_BY_SIZE(cl_int)
    MAKE_INT_BY_SIZE(cl_long)

    template<typename TYPE> struct type_to_datatype {};
#define MAKE_TYPE_TO_DATATYPE(TYPE, DATA_TYPE) template<> struct type_to_datatype<TYPE> {const static datatype_t datatype = DATA_TYPE;};
    /// Wrapper Objects

    template<typename TYPE, property_t ID>
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

    template<typename TYPE, property_t ID>
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

        // private node ptr
        Node* _node;

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
            // swizzles
            property_rw<scalar<CL_TYPE>, Property::X> X;
            property_rw<scalar<CL_TYPE>, Property::Y> Y;
            property_r<vector2<CL_TYPE>, Property::XX> XX;
            property_rw<vector2<CL_TYPE>, Property::XY> XY;
            property_rw<vector2<CL_TYPE>, Property::YX> YX;
            property_r<vector2<CL_TYPE>, Property::YY> YY;
            // others
            property_rw<scalar<CL_TYPE>, Property::Lo> Lo;
            property_rw<scalar<CL_TYPE>, Property::Hi> Hi;
            property_rw<scalar<CL_TYPE>, Property::Even> Even;
            property_rw<scalar<CL_TYPE>, Property::Odd> Odd;

            // private node ptr
            Node* _node;
        };

        scalar<CL_TYPE> operator[](const scalar<cl_int>& index)
        {
            UNREFERENCED_PARAMETER(index);
            TRACE
            return scalar<CL_TYPE>();
        }

        const scalar<CL_TYPE> operator[](const scalar<cl_int>& index) const
        {
            UNREFERENCED_PARAMETER(index);
            TRACE
            return scalar<CL_TYPE>();
        }

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

    #define MAKE_UNARY_OPERATOR(RETURN_TYPE, TYPE, OP, ENUM)\
    RETURN_TYPE operator OP(const TYPE& right)\
    {\
        TRACE;\
        RETURN_TYPE result;\
        result._node = spark_create_function_node(type_to_datatype<RETURN_TYPE>::datatype, (symbolid_t)Function::ENUM);\
        spark_add_child_node(result._node, right._node);\
        return result;\
    }

    #define MAKE_BINARY_OPERATOR(RETURN_TYPE, TYPE, OP, ENUM)\
    RETURN_TYPE operator OP (const TYPE& left, const TYPE& right)\
    {\
        TRACE;\
        RETURN_TYPE result;\
        result._node = spark_create_function_node(type_to_datatype<RETURN_TYPE>::datatype, (symbolid_t)Function::ENUM);\
        spark_add_child_node(result._node, left._node);\
        spark_add_child_node(result._node, right._node);\
        return result;\
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
    MAKE_BINARY_OPERATOR(TYPE, TYPE, <<, LeftShift)

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
    MAKE_UNARY_OPERATOR(TYPE, TYPE, ~, BitwiseNot)\
    MAKE_BINARY_OPERATOR(TYPE, TYPE, &, BitwiseAnd)\
    MAKE_BINARY_OPERATOR(TYPE, TYPE, |, BitwiseOr)\
    MAKE_BINARY_OPERATOR(TYPE, TYPE, ^, BitwiseXor)\
    MAKE_BINARY_OPERATOR(TYPE, TYPE, >>, RightShift)\
    MAKE_BINARY_OPERATOR(TYPE, TYPE, <<, LeftShift)

    #define MAKE_INT_TYPES(TYPE, CL_TYPE)\
    typedef scalar<CL_TYPE> TYPE;\
    typedef vector2<CL_TYPE> TYPE##2;\
    MAKE_TYPE_TO_DATATYPE(TYPE, DataType::TYPE);\
    MAKE_TYPE_TO_DATATYPE(TYPE##2, (datatype_t)(DataType::TYPE | DataType::Vector2))\
    MAKE_INT_OPERATORS(TYPE)\
    MAKE_INT_OPERATORS(TYPE##2)

    #define MAKE_FLOAT_TYPES(TYPE, CL_TYPE)\
    typedef scalar<CL_TYPE> TYPE;\
    typedef vector2<CL_TYPE> TYPE##2;\
    MAKE_TYPE_TO_DATATYPE(TYPE, DataType::TYPE);\
    MAKE_TYPE_TO_DATATYPE(TYPE##2, (datatype_t)(DataType::TYPE | DataType::Vector2))\
    MAKE_FLOAT_OPERATORS(TYPE)\
    MAKE_FLOAT_OPERATORS(TYPE##2)

    // Int has to appear first since it is a return type of other scalar comparison operators
    MAKE_INT_TYPES(Int, cl_int)
    // other ints
    MAKE_INT_TYPES(Char, cl_char)
    MAKE_INT_TYPES(UChar, cl_uchar)
    MAKE_INT_TYPES(Short, cl_short)
    MAKE_INT_TYPES(UShort, cl_ushort)
    MAKE_INT_TYPES(UInt, cl_uint)
    MAKE_INT_TYPES(Long, cl_long)
    MAKE_INT_TYPES(ULong, cl_ulong)

    // floating point
    MAKE_FLOAT_TYPES(Float, cl_float)
    MAKE_FLOAT_TYPES(Double, cl_double)

}