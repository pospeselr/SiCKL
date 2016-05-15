#pragma once

namespace Spark
{
     /// Type Translations

    // gets the equivalent integer type (used for comparison operators)
    template<typename T> struct type_to_int {typedef void type;};
#define MAKE_TYPE_TO_INT(TYPE, INT_TYPE)\
    template<> struct type_to_int<TYPE> {typedef INT_TYPE type;};\
    template<> struct type_to_int<TYPE##2> {typedef INT_TYPE##2 type;};\
    template<> struct type_to_int<TYPE##4> {typedef INT_TYPE##4 type;};\
    template<> struct type_to_int<TYPE##8> {typedef INT_TYPE##8 type;};\
    template<> struct type_to_int<TYPE##16> {typedef INT_TYPE##16 type;};

    MAKE_TYPE_TO_INT(cl_char, cl_char)
    MAKE_TYPE_TO_INT(cl_uchar, cl_char)
    MAKE_TYPE_TO_INT(cl_short, cl_short)
    MAKE_TYPE_TO_INT(cl_ushort, cl_short)
    MAKE_TYPE_TO_INT(cl_int, cl_int)
    MAKE_TYPE_TO_INT(cl_uint, cl_int)
    MAKE_TYPE_TO_INT(cl_long, cl_long)
    MAKE_TYPE_TO_INT(cl_ulong, cl_long)
    MAKE_TYPE_TO_INT(cl_float, cl_int)
    MAKE_TYPE_TO_INT(cl_double, cl_long)

    template<typename TYPE> struct type_to_datatype {};
#define MAKE_TYPE_TO_DATATYPE(TYPE, DATA_TYPE) template<> struct type_to_datatype<TYPE> {const static datatype_t datatype = DATA_TYPE;};
    /// Wrapper Objects

    template<typename TYPE, property_t ID>
    struct property_r
    {
        operator TYPE() const
        {
            return TYPE();
        };

        const TYPE operator()() const
        {
            return TYPE();
        }
    };

    template<typename TYPE, property_t ID>
    struct property_rw
    {
        operator TYPE() const
        {
            return TYPE();
        };

        property_rw& operator=(const TYPE& right)
        {
            UNREFERENCED_PARAMETER(right);
            return *this;
        }
    };

    // default constructor
    template<typename TYPE, typename CL_TYPE>
    void default_constructor(TYPE* type)
    {
        const auto dt = type_to_datatype<TYPE>::datatype;

        Node* thisNode = spark_create_symbol_node(dt, spark_next_symbol());
        type->_node = thisNode;

        // init to 0
        uint8_t raw[sizeof(CL_TYPE)] = {0};
        Node* valNode = spark_create_constant_node(dt, raw, sizeof(raw));

        // create assignment node
        Node* assignmentNode = spark_create_function_node(dt, (symbolid_t)Function::Assignment);
        spark_add_child_node(assignmentNode, thisNode);
        spark_add_child_node(assignmentNode, valNode);

        // add to tree
        Node* currentScope = spark_peek_scope_node();
        spark_add_child_node(currentScope, assignmentNode);
    }

    // value constructor
    template<typename TYPE, typename CL_TYPE>
    void value_constructor(TYPE* type, const CL_TYPE& val)
    {
        const auto dt = type_to_datatype<TYPE>::datatype;

        Node* thisNode = spark_create_symbol_node(dt, spark_next_symbol());
        type->_node = thisNode;

        // init to val
        Node* valNode = spark_create_constant_node(dt, &val, sizeof(val));

        // create assignment node
        Node* assignmentNode = spark_create_function_node(dt, (symbolid_t)Function::Assignment);
        spark_add_child_node(assignmentNode, thisNode);
        spark_add_child_node(assignmentNode, valNode);

        // add to tree
        Node* currentScope = spark_peek_scope_node();
        spark_add_child_node(currentScope, assignmentNode);
    }

    template<typename TYPE>
    void assignment_operator(TYPE* type, const TYPE& that)
    {
        const auto dt = type_to_datatype<TYPE>::datatype;

        // create assignment node
        Node* assignmentNode = spark_create_function_node(dt, (symbolid_t)Function::Assignment);
        spark_add_child_node(assignmentNode, type->_node);
        spark_add_child_node(assignmentNode, that._node);

        // add to tree
        Node* currentScope = spark_peek_scope_node();
        spark_add_child_node(currentScope, assignmentNode);
    }

    template<typename TYPE, typename CL_SCALAR, size_t N>
    void assignment_operator(TYPE* type, const CL_SCALAR (&that)[N])
    {
        const auto dt = type_to_datatype<TYPE>::datatype;

        // init to val
        Node* thisNode = type->_node;
        Node* valNode = spark_create_constant_node(dt, &that, sizeof(that));

        // create assignment node
        Node* assignmentNode = spark_create_function_node(dt, (symbolid_t)Function::Assignment);
        spark_add_child_node(assignmentNode, thisNode);
        spark_add_child_node(assignmentNode, valNode);

        // add to tree
        Node* currentScope = spark_peek_scope_node();
        spark_add_child_node(currentScope, assignmentNode);
    }

    // scalar type wrapper
    template<typename CL_TYPE>
    struct scalar
    {
        // constructors
        scalar(Node* node)
        : _node(node)
        {
            SPARK_ASSERT(_node != nullptr);
        }

        scalar()
        {
            default_constructor<scalar<CL_TYPE>, CL_TYPE>(this);
            SPARK_ASSERT(_node != nullptr);
        }

        scalar(CL_TYPE val)
        {
            value_constructor<scalar<CL_TYPE>, CL_TYPE>(this, val);
            SPARK_ASSERT(_node != nullptr);
        }

        scalar& operator=(const scalar& that)
        {
            assignment_operator<scalar<CL_TYPE>>(this, that);
            SPARK_ASSERT(_node != nullptr);
            return *this;
        }

        scalar& operator=(CL_TYPE val)
        {
            assignment_operator<scalar<CL_TYPE>, CL_TYPE, 1>(this, {val});
            SPARK_ASSERT(_node != nullptr);
            return *this;
        }

        // cast operators

        template<typename S>
        explicit operator scalar<S>() const
        {
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
        typedef decltype(CL_TYPE::x) CL_SCALAR;
        typedef CL_TYPE CL_VECTOR2;

        // constructors
        vector2(Node* node)
        : _node(node)
        {
            SPARK_ASSERT(_node != nullptr);
        }

        vector2()
        {
            default_constructor<vector2<CL_VECTOR2>, CL_VECTOR2>(this);
            SPARK_ASSERT(_node != nullptr);
        }

        vector2(const CL_TYPE& val)
        {
            value_constructor<vector2<CL_VECTOR2>, CL_VECTOR2>(this, val);
            SPARK_ASSERT(_node != nullptr);
        }

        vector2& operator=(const vector2& that)
        {
            assignment_operator<vector2<CL_VECTOR2>>(this, that);
            SPARK_ASSERT(_node != nullptr);
            return *this;
        }

        vector2& operator=(const CL_SCALAR (&val)[2])
        {
            assignment_operator<vector2<CL_VECTOR2>, CL_SCALAR, 2>(this, val);
            SPARK_ASSERT(_node != nullptr);
            return *this;
        }

        // properties
        union
        {
            // swizzles
            property_rw<scalar<CL_SCALAR>, Property::X> X;
            property_rw<scalar<CL_SCALAR>, Property::Y> Y;
            property_r<vector2<CL_VECTOR2>, Property::XX> XX;
            property_rw<vector2<CL_VECTOR2>, Property::XY> XY;
            property_rw<vector2<CL_VECTOR2>, Property::YX> YX;
            property_r<vector2<CL_VECTOR2>, Property::YY> YY;
            // others
            property_rw<scalar<CL_SCALAR>, Property::Lo> Lo;
            property_rw<scalar<CL_SCALAR>, Property::Hi> Hi;
            property_rw<scalar<CL_SCALAR>, Property::Even> Even;
            property_rw<scalar<CL_SCALAR>, Property::Odd> Odd;

            // private node ptr
            Node* _node;
        };

        scalar<CL_SCALAR> operator[](const scalar<cl_int>& index)
        {
            UNREFERENCED_PARAMETER(index);
            return scalar<CL_SCALAR>();
        }

        const scalar<CL_SCALAR> operator[](const scalar<cl_int>& index) const
        {
            UNREFERENCED_PARAMETER(index);
            return scalar<CL_SCALAR>();
        }

        // cast operators
        template<typename S>
        explicit operator vector2<S>() const
        {
            return vector2<S>();
        }

        typedef vector2<typename type_to_int<CL_VECTOR2>::type> int_type;
    };

    template<typename T>
    using int_type = typename T::int_type;

    /// Operators

    #define MAKE_UNARY_OPERATOR(RETURN_TYPE, TYPE, OP, ENUM)\
    RETURN_TYPE operator OP(const TYPE& right)\
    {\
        Node* result_node = spark_create_function_node(type_to_datatype<RETURN_TYPE>::datatype, (symbolid_t)Function::ENUM);\
        spark_add_child_node(result_node, right._node);\
        return RETURN_TYPE(result_node);\
    }

    #define MAKE_BINARY_OPERATOR(RETURN_TYPE, TYPE, OP, ENUM)\
    RETURN_TYPE operator OP (const TYPE& left, const TYPE& right)\
    {\
        Node* result_node = spark_create_function_node(type_to_datatype<RETURN_TYPE>::datatype, (symbolid_t)Function::ENUM);\
        spark_add_child_node(result_node, left._node);\
        spark_add_child_node(result_node, right._node);\
        return RETURN_TYPE(result_node);\
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
    typedef vector2<CL_TYPE##2> TYPE##2;\
    MAKE_TYPE_TO_DATATYPE(TYPE, DataType::TYPE);\
    MAKE_TYPE_TO_DATATYPE(TYPE##2, (datatype_t)(DataType::TYPE | DataType::Vector2))\
    MAKE_INT_OPERATORS(TYPE)\
    MAKE_INT_OPERATORS(TYPE##2)

    #define MAKE_FLOAT_TYPES(TYPE, CL_TYPE)\
    typedef scalar<CL_TYPE> TYPE;\
    typedef vector2<CL_TYPE##2> TYPE##2;\
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