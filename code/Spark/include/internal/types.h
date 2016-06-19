#pragma once

namespace Spark
{
    /// Type properties

    template<typename T> struct is_scalar_type {const static bool value = false;};
    template<typename T> struct is_vector2_type {const static bool value = false;};

     /// Type Translations

    // gets the equivalent integer type (used for comparison operators)
    template<typename T> struct type_to_int {typedef void type;};
#define MAKE_TYPE_TO_INT(TYPE, INT_TYPE)\
    template<> struct type_to_int<TYPE> {typedef cl_int type;};\
    template<> struct type_to_int<TYPE##2> {typedef INT_TYPE##2 type;};\
    template<> struct type_to_int<TYPE##4> {typedef INT_TYPE##4 type;};\
    template<> struct type_to_int<TYPE##8> {typedef INT_TYPE##8 type;};\
    template<> struct type_to_int<TYPE##16> {typedef INT_TYPE##16 type;}

    MAKE_TYPE_TO_INT(cl_char, cl_char);
    MAKE_TYPE_TO_INT(cl_uchar, cl_char);
    MAKE_TYPE_TO_INT(cl_short, cl_short);
    MAKE_TYPE_TO_INT(cl_ushort, cl_short);
    MAKE_TYPE_TO_INT(cl_int, cl_int);
    MAKE_TYPE_TO_INT(cl_uint, cl_int);
    MAKE_TYPE_TO_INT(cl_long, cl_long);
    MAKE_TYPE_TO_INT(cl_ulong, cl_long);
    MAKE_TYPE_TO_INT(cl_float, cl_int);
    MAKE_TYPE_TO_INT(cl_double, cl_long);

    template<typename TYPE> struct type_to_datatype {};
#define MAKE_TYPE_TO_DATATYPE(TYPE, DATA_TYPE) template<> struct type_to_datatype<TYPE> {const static datatype_t datatype = DATA_TYPE;};
    /// Wrapper Objects

    template<typename TYPE, bool DESTRUCT_ATTACH=false>
    struct rvalue : TYPE
    {
        rvalue(Node* node) : TYPE(node) {}
        ~rvalue()
        {
            // used for prefix/postix operator (or any operator which has side effects and
            // also returns an rvalue which may be ignored)

            // from 12.2/3:

            // Temporary objects are destroyed as the last step in evaluating the full-
            // expression (1.9) that (lexically) contains the point where they were created.

            if(DESTRUCT_ATTACH &&
               this->_node->_attached == false)
            {
                // add to tree
                Node* currentScope = spark_peek_scope_node();
                spark_add_child_node(currentScope, this->_node);
            }
        }
        rvalue& operator=(const rvalue&) = delete;
    };

    template<typename TYPE, property_t ID>
    struct property_r
    {
        operator const rvalue<TYPE>() const
        {
            Node* property = spark_create_property_node(ID);
            const auto dt = type_to_datatype<TYPE>::datatype;
            const auto op = Operator::Property;

            return rvalue<TYPE>(spark_create_operator2_node(dt, op, this->_node, property));
        };

        const rvalue<TYPE> operator()() const
        {
            Node* property = spark_create_property_node(ID);
            const auto dt = type_to_datatype<TYPE>::datatype;
            const auto op = Operator::Property;

            return rvalue<TYPE>(spark_create_operator2_node(dt, op, this->_node, property));
        }
        Node* _node;
    };

    template<typename TYPE, property_t ID>
    struct property_rw
    {
        operator TYPE() const
        {
            Node* property = spark_create_property_node(ID);
            const auto dt = type_to_datatype<TYPE>::datatype;
            const auto op = Operator::Property;

            return TYPE(spark_create_operator2_node(dt, op, this->_node, property));
        };

        TYPE operator()() const
        {
            Node* property = spark_create_property_node(ID);
            const auto dt = type_to_datatype<TYPE>::datatype;
            const auto op = Operator::Property;

            return TYPE(spark_create_operator2_node(dt, op, this->_node, property));
        }

        property_rw& operator=(const TYPE& right)
        {
            // create property node
            Node* property = spark_create_property_node(ID);
            Node* lvalue = spark_create_operator2_node(type_to_datatype<TYPE>::datatype, Operator::Property, this->_node, property);

            // create assignment node
            const auto dt = type_to_datatype<TYPE>::datatype;
            const auto op = Operator::Assignment;
            Node* assignmentNode = spark_create_operator2_node(dt, op, lvalue, right._node);

            // add to tree
            Node* currentScope = spark_peek_scope_node();
            spark_add_child_node(currentScope, assignmentNode);

            return *this;
        }
        Node* _node;
    };

    // variable declaration 'constructor' (used for symbols passed into functions with no defined value)
    template<typename TYPE>
    void extern_constructor(TYPE* pThis)
    {
        const auto dt = type_to_datatype<TYPE>::datatype;

        Node* thisNode = spark_create_symbol_node(dt, spark_next_symbol());
        pThis->_node = thisNode;
    }

    // value constructor
    template<typename TYPE, size_t SIZE>
    void value_constructor(TYPE* pThis, const void* raw)
    {
        const auto dt = type_to_datatype<TYPE>::datatype;
        const auto op = Operator::Assignment;

        Node* thisNode = spark_create_symbol_node(dt, spark_next_symbol());
        pThis->_node = thisNode;

        // init to val
        Node* valNode = spark_create_constant_node(dt, raw, SIZE);

        // create assignment node
        Node* assignmentNode = spark_create_operator2_node(dt, op, thisNode, valNode);
        // add to tree
        Node* currentScope = spark_peek_scope_node();
        spark_add_child_node(currentScope, assignmentNode);
    }

    // default constructor (ie 0)
    template<typename TYPE, typename CL_TYPE>
    void default_constructor(TYPE* pThis)
    {
        // init to 0
        CL_TYPE val = {0};
        value_constructor<TYPE, sizeof(val)>(pThis, &val);
    }

    // copy constructor
    template<typename TYPE>
    void copy_constructor(TYPE* pThis, const TYPE& that)
    {
        const auto dt = type_to_datatype<TYPE>::datatype;
        const auto op = Operator::Assignment;

        Node* thisNode = spark_create_symbol_node(dt, spark_next_symbol());
        pThis->_node = thisNode;

        Node* thatNode = that._node;

        // create assignment node
        Node* assignmentNode = spark_create_operator2_node(dt, op, thisNode, thatNode);
        // add to tree
        Node* currentScope = spark_peek_scope_node();
        spark_add_child_node(currentScope, assignmentNode);
    }

    // assignment operator for types
    template<typename TYPE>
    void assignment_operator(TYPE* pThis, const TYPE& that)
    {
        SPARK_ASSERT((pThis->_node->_type == NodeType::Symbol) ||
                     (pThis->_node->_type == NodeType::Operator && pThis->_node->_function.id == Operator::Index));

        const auto dt = type_to_datatype<TYPE>::datatype;
        const auto op = Operator::Assignment;

        // create assignment node
        Node* assignmentNode = spark_create_operator2_node(dt, op, pThis->_node, that._node);
        // add to tree
        Node* currentScope = spark_peek_scope_node();
        spark_add_child_node(currentScope, assignmentNode);
    }

    // assignment operator for literals
    template<typename TYPE, size_t SIZE>
    void assignment_operator(TYPE* pThis, const void* raw)
    {
        SPARK_ASSERT((pThis->_node->_type == NodeType::Symbol) ||
                     (pThis->_node->_type == NodeType::Operator && pThis->_node->_function.id == Operator::Index));

        const auto dt = type_to_datatype<TYPE>::datatype;
        const auto op = Operator::Assignment;

        // init to val
        Node* thisNode = pThis->_node;
        Node* valNode = spark_create_constant_node(dt, raw, SIZE);

        // create assignment node
        Node* assignmentNode = spark_create_operator2_node(dt, op, thisNode, valNode);
        // add to tree
        Node* currentScope = spark_peek_scope_node();
        spark_add_child_node(currentScope, assignmentNode);
    }

    // forward declare tyeps
    template<typename CL_TYPE> struct scalar;
    template<typename CL_TYPE> struct vector2;

    // scalar type wrapper
    template<typename CL_TYPE>
    struct scalar
    {
        friend struct rvalue<scalar, true>;
        friend struct rvalue<scalar, false>;
        template<typename S>
        friend struct scalar;
        template<typename S>
        friend struct vector2;
        template<typename S, property_t id>
        friend struct property_rw;
    private:
        // node constructor
        scalar(Node* node)
        : _node(node)
        {
            SPARK_ASSERT(_node != nullptr);
        }
    public:
        // constructors
        scalar()
        {
            default_constructor<scalar<CL_TYPE>, CL_TYPE>(this);
            SPARK_ASSERT(_node != nullptr);
        }

        scalar(nullptr_t)
        {
            extern_constructor(this);
        }

        scalar(CL_TYPE val)
        {
            value_constructor<scalar<CL_TYPE>, sizeof(CL_TYPE)>(this, &val);
            SPARK_ASSERT(_node != nullptr);
        }

        scalar(const scalar& that)
        {
            copy_constructor<scalar<CL_TYPE>>(this, that);
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
            assignment_operator<scalar<CL_TYPE>, sizeof(val)>(this, &val);
            SPARK_ASSERT(_node != nullptr);
            return *this;
        }

        // cast operator

        template<typename T>
        rvalue<T> As() const
        {
            static_assert(is_scalar_type<T>::value, "scalar types can only be cast to other scalar types");

            const auto dt = type_to_datatype<T>::datatype;
            const auto op = Operator::Cast;

            return rvalue<T>(spark_create_operator1_node(dt, op, this->_node));
        }

        // private node ptr
        Node* _node;

        typedef scalar<typename type_to_int<CL_TYPE>::type> int_type;
    };
    template<typename T> struct is_scalar_type<scalar<T>> {const static bool value = true;};

    // wrapper for all vector2 types
    template<typename CL_TYPE>
    struct vector2
    {
        typedef decltype(CL_TYPE::x) CL_SCALAR;
        typedef CL_TYPE CL_VECTOR2;

        friend struct rvalue<vector2, true>;
        friend struct rvalue<vector2, false>;
        template<typename S, property_t id>
        friend struct property_rw;
    private:
        // node constructor
        vector2(Node* node)
        : _node(node)
        {
            SPARK_ASSERT(_node != nullptr);
        }
    public:
        // constructors
        vector2()
        {
            default_constructor<vector2<CL_VECTOR2>, CL_VECTOR2>(this);
            SPARK_ASSERT(_node != nullptr);
        }

        vector2(nullptr_t)
        {
            extern_constructor(this);
        }

        vector2(const CL_TYPE& val)
        {
            value_constructor<vector2<CL_VECTOR2>, sizeof(val)>(this, &val);
            SPARK_ASSERT(_node != nullptr);
        }

        vector2(const CL_SCALAR (&val)[2])
        {
            value_constructor<vector2<CL_VECTOR2>, 2 * sizeof(CL_SCALAR)>(this, &val[0]);
            SPARK_ASSERT(_node != nullptr);
        }

        vector2(const vector2& that)
        {
            copy_constructor<vector2<CL_TYPE>>(this, that);
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
            assignment_operator<vector2<CL_VECTOR2>, 2 * sizeof(CL_SCALAR)>(this, &val[0]);
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
            const auto dt = type_to_datatype<scalar<CL_SCALAR>>::datatype;
            const auto op = Operator::Index;

            Node* indexNode = spark_create_operator2_node(dt, op, this->_node, index._node);
            return scalar<CL_SCALAR>(indexNode);
        }

        const rvalue<scalar<CL_SCALAR>> operator[](const scalar<cl_int>& index) const
        {
            const auto dt = type_to_datatype<scalar<CL_SCALAR>>::datatype;
            const auto op = Operator::Index;

            Node* indexNode = spark_create_operator2_node(dt, op, this->_node, index._node);
            return rvalue<scalar<CL_SCALAR>>(indexNode);
        }

        scalar<CL_SCALAR> operator[](cl_int index)
        {
            SPARK_ASSERT(index == 0 || index == 1);

            const auto dt = type_to_datatype<scalar<CL_SCALAR>>::datatype;
            const auto op = Operator::Index;

            Node* valNode = spark_create_constant_node(DataType::Int, &index, sizeof(index));
            Node* indexNode = spark_create_operator2_node(dt, op, this->_node, valNode);
            return scalar<CL_SCALAR>(indexNode);
        }

        const rvalue<scalar<CL_SCALAR>> operator[](cl_int index) const
        {
            SPARK_ASSERT(index == 0 || index == 1);

            const auto dt = type_to_datatype<scalar<CL_SCALAR>>::datatype;
            const auto op = Operator::Index;

            Node* valNode = spark_create_constant_node(DataType::Int, &index, sizeof(index));
            Node* indexNode = spark_create_operator2_node(dt, op, this->_node, valNode);
            return rvalue<scalar<CL_SCALAR>>(indexNode);
        }

        // cast operator
        template<typename T>
        rvalue<T> As() const
        {
            static_assert(is_vector2_type<T>::value, "vector2 types can only be cast to other vector2 types");

            const auto dt = type_to_datatype<T>::datatype;
            const auto op = Operator::Cast;

            return rvalue<T>(spark_create_operator1_node(dt, op, this->_node));
        }

        typedef vector2<typename type_to_int<CL_VECTOR2>::type> int_type;
    };
    template<typename T> struct is_vector2_type<vector2<T>> {const static bool value = true;};


    /// int_type used to determine what boolean operators should return

    template<typename T>
    using int_type = typename T::int_type;

    /// Operators

    #define MAKE_UNARY_OPERATOR(RETURN_TYPE, TYPE, OP, ENUM)\
    const rvalue<RETURN_TYPE> operator OP(const TYPE& right)\
    {\
        const auto dt = type_to_datatype<RETURN_TYPE>::datatype;\
        const auto op = Operator::ENUM;\
        return rvalue<RETURN_TYPE>(spark_create_operator1_node(dt, op, right._node));\
    }

    #define MAKE_BINARY_OPERATOR(RETURN_TYPE, TYPE, OP, ENUM)\
    const rvalue<RETURN_TYPE> operator OP (const TYPE& left, const TYPE& right)\
    {\
        const auto dt = type_to_datatype<RETURN_TYPE>::datatype;\
        const auto op = Operator::ENUM;\
        return rvalue<RETURN_TYPE>(spark_create_operator2_node(dt, op, left._node, right._node));\
    }

    #define MAKE_PREFIX_OPERATOR(RETURN_TYPE, TYPE, OP, ENUM)\
    const rvalue<RETURN_TYPE, true> operator OP (const TYPE& value)\
    {\
        const auto dt = type_to_datatype<RETURN_TYPE>::datatype;\
        const auto op = Operator::ENUM;\
        return rvalue<RETURN_TYPE, true>(spark_create_operator1_node(dt, op, value._node));\
    }

    #define MAKE_POSTFIX_OPERATOR(RETURN_TYPE, TYPE, OP, ENUM)\
    const rvalue<RETURN_TYPE, true> operator OP (const TYPE& value, int)\
    {\
        const auto dt = type_to_datatype<RETURN_TYPE>::datatype;\
        const auto op = Operator::ENUM;\
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
    MAKE_POSTFIX_OPERATOR(TYPE, TYPE, --, PostfixDecrement)

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