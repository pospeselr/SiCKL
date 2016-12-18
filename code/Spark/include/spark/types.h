#pragma once

namespace Spark
{
    namespace Internal
    {
        inline
        __attribute__ ((noinline))
        void assignment_operator(Node* thisNode, datatype_t dt, Node* thatNode)
        {
            const auto type= spark_node_get_type(thisNode, Spark::Internal::ThrowOnError());
            const auto operatorId = spark_node_get_operator_id(thisNode, Spark::Internal::ThrowOnError());

            SPARK_ASSERT((type == NodeType::Symbol) ||
                         (type == NodeType::Operator && operatorId == Operator::Index) ||
                         (type == NodeType::Operator && operatorId == Operator::Dereference) ||
                         (type == NodeType::Operator && operatorId == Operator::Property));

            const auto op = Operator::Assignment;

            // create assignment node
            Node* assignmentNode = spark_create_operator2_node(dt, op, thisNode, thatNode);
            // add to tree
            Node* currentScope = spark_peek_scope_node(Spark::Internal::ThrowOnError());
            spark_add_child_node(currentScope, assignmentNode, Spark::Internal::ThrowOnError());
        }

        inline
        __attribute__ ((noinline))
        void assignment_operator(Node* thisNode, datatype_t dt, const void* raw, size_t sz)
        {
            const auto type = spark_node_get_type(thisNode, Spark::Internal::ThrowOnError());
            const auto operatorId = spark_node_get_operator_id(thisNode, Spark::Internal::ThrowOnError());

            SPARK_ASSERT((type == NodeType::Symbol) ||
                         (type == NodeType::Operator && operatorId == Operator::Index) ||
                         (type == NodeType::Operator && operatorId == Operator::Dereference) ||
                         (type == NodeType::Operator && operatorId == Operator::Property));

            const auto op = Operator::Assignment;

            // init to val
            Node* valNode = spark_create_constant_node(dt, raw, sz, Spark::Internal::ThrowOnError());

            // create assignment node
            Node* assignmentNode = spark_create_operator2_node(dt, op, thisNode, valNode);
            // add to tree
            Node* currentScope = spark_peek_scope_node(Spark::Internal::ThrowOnError());
            spark_add_child_node(currentScope, assignmentNode, Spark::Internal::ThrowOnError());
        }
    }

    /// Type properties

    template<typename T> struct is_scalar_type {const static bool value = false;};
    template<typename T> struct is_vector2_type {const static bool value = false;};

     /// Type Translations

    // gets the equivalent integer type (used for comparison operators)
    template<typename T> struct type_to_signed_int {typedef void type;};

#define MAKE_TYPE_TO_INT(TYPE, INT_TYPE)\
    template<> struct type_to_signed_int<TYPE> {typedef INT_TYPE type;};\

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

    // assignment operator for types
    template<typename TYPE>
    void assignment_operator(TYPE* pThis, const TYPE& that)
    {
        Internal::assignment_operator(pThis->_node, TYPE::type, that._node);
    }

    // assignment operator for literals
    template<typename TYPE, size_t SIZE>
    void assignment_operator(TYPE* pThis, const void* raw)
    {
        Internal::assignment_operator(pThis->_node, TYPE::type, raw, SIZE);
    }

    // forward declare tyeps
    template<typename RAW_TYPE> struct scalar;
    template<typename TYPE> struct vector2;

    // pointer type
    template<typename TYPE>
    struct Pointer
    {
    protected:
        Pointer(Node* node)
        : _node(node)
        {
            SPARK_ASSERT(_node != nullptr);
        }
    public:
        Pointer(std::nullptr_t)
        {
            extern_constructor(this);
        }

        Pointer(const Pointer& that)
        {
            copy_constructor<Pointer>(this, that);
            SPARK_ASSERT(_node != nullptr);
        }

        Pointer(Pointer&&) = default;

        Pointer& operator=(const Pointer& that)
        {
            assignment_operator<Pointer>(this, that);
            SPARK_ASSERT(_node != nullptr);
            return *this;
        }

        // pointer dereference operator
        TYPE operator*()
        {
            const auto dt = TYPE::type;
            const auto op = Operator::Dereference;
            return TYPE(spark_create_operator1_node(dt, op, this->_node));
        }
        rvalue<TYPE> operator*() const
        {
            TRACE
            const auto dt = TYPE::type;
            const auto op = Operator::Dereference;
            return rvalue<TYPE>(spark_create_operator1_node(dt, op, this->_node));
        }

        Node* _node = nullptr;
        friend TYPE;

        static const char* name;
        static const datatype_t type;
    };

    // scalar type wrapper
    template<typename RAW_TYPE>
    struct scalar
    {
    public:
        // typedefs
        typedef scalar<int32_t> int_type;
        typedef RAW_TYPE raw_type;

        // friends
        template<typename S>
        friend struct scalar;
        template<typename S>
        friend struct vector2;
        template<typename S, property_t id>
        friend struct property_rw;
        template<typename S>
        friend struct Pointer;
    protected:
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
            RAW_TYPE val = {};
            this->_node = Internal::value_constructor(scalar::type, &val, sizeof(val));
        }

        scalar(std::nullptr_t)
        {
            this->_node = Internal::extern_constructor(scalar::type);
        }

        scalar(RAW_TYPE val)
        {
            this->_node = Internal::value_constructor(scalar::type, &val, sizeof(val));
        }

        scalar(const scalar& that)
        {
            this->_node = Internal::copy_constructor(scalar::type, that._node);
        }

        scalar(const lvalue<scalar>& that)
        {
            this->_node = Internal::copy_constructor(scalar::type, that._node);
        }

        scalar(const rvalue<scalar>& that)
        {
            this->_node = Internal::copy_constructor(scalar::type, that._node);
        }

        scalar(scalar&&) = default;

        scalar& operator=(const scalar& that)
        {
            Internal::assignment_operator(this->_node, scalar::type, that._node);
            return *this;
        }

        scalar& operator=(RAW_TYPE val)
        {
            Internal::assignment_operator(this->_node, scalar::type, &val, sizeof(val));
            return *this;
        }

        // cast operator

        template<typename CAST_TYPE>
        const rvalue<CAST_TYPE> As() const
        {
            static_assert(is_scalar_type<CAST_TYPE>::value, "scalar types can only be cast to other scalar types");

            const auto dt = CAST_TYPE::type;
            const auto op = Operator::Cast;
            return rvalue<CAST_TYPE>(spark_create_operator1_node(dt, op, this->_node));
        }

        // private node ptr
        Node* _node;
        static const char* name;
        static const datatype_t type;
    };
    template<typename T> struct is_scalar_type<scalar<T>> {const static bool value = true;};

    // wrapper for all vector2 types
    template<typename TYPE>
    struct vector2
    {
    public:
        // typedefs
        typedef vector2<scalar<typename type_to_signed_int<typename TYPE::raw_type>::type>> int_type;

        // friends
        template<typename S, property_t id>
        friend struct property_rw;
    protected:
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
            typename TYPE::raw_type val[2] = {};
            this->_node = Internal::value_constructor(vector2::type, &val[0], sizeof(val));
        }

        vector2(std::nullptr_t)
        {
            this->_node = Internal::extern_constructor(vector2::type);
        }

        vector2(const rvalue<vector2>& that)
        {
            this->_node = Internal::copy_constructor(vector2::type, that._node);
        }

        vector2(const lvalue<vector2>& that)
        {
            this->_node = Internal::copy_constructor(vector2::type, that._node);
        }

        vector2(const typename TYPE::raw_type (&val)[2])
        {
            this->_node = Internal::value_constructor(vector2::type, &val[0], sizeof(val));
        }

        vector2(const vector2& that)
        {
            this->_node = Internal::copy_constructor(vector2::type, that._node);
        }

        vector2(vector2&&) = default;

        vector2& operator=(const vector2& that)
        {
            Internal::assignment_operator(this->_node, vector2::type, that._node);
            return *this;
        }

        vector2& operator=(const typename TYPE::raw_type (&val)[2])
        {
            Internal::assignment_operator(this->_node, vector2::type, &val[0], sizeof(val));
            return *this;
        }

        // properties
        union
        {
            // swizzles
            property_rw<TYPE, Property::X> X;
            property_rw<TYPE, Property::Y> Y;
            property_r<vector2, Property::XX> XX;
            property_rw<vector2, Property::XY> XY;
            property_rw<vector2, Property::YX> YX;
            property_r<vector2, Property::YY> YY;
            // others
            property_rw<TYPE, Property::Lo> Lo;
            property_rw<TYPE, Property::Hi> Hi;
            property_rw<TYPE, Property::Even> Even;
            property_rw<TYPE, Property::Odd> Odd;

            // private node ptr
            Node* _node;
        };

        // indexing operators
        lvalue<TYPE> operator[](const rvalue<scalar<cl_int>>& index)
        {
            const auto dt = TYPE::type;
            const auto op = Operator::Index;

            Node* indexNode = spark_create_operator2_node(dt, op, this->_node, index._node);
            return lvalue<TYPE>(indexNode);
        }

        const rvalue<TYPE> operator[](const rvalue<scalar<cl_int>>& index) const
        {
            const auto dt = TYPE::type;
            const auto op = Operator::Index;

            Node* indexNode = spark_create_operator2_node(dt, op, this->_node, index._node);
            return rvalue<TYPE>(indexNode);
        }

        // cast operator
        template<typename CAST_TYPE>
        const rvalue<CAST_TYPE> As() const
        {
            static_assert(is_vector2_type<CAST_TYPE>::value, "vector2 types can only be cast to other vector2 types");

            const auto dt = CAST_TYPE::type;
            const auto op = Operator::Cast;

            return rvalue<CAST_TYPE>(spark_create_operator1_node(dt, op, this->_node));
        }

        static const char* name;
        static const datatype_t type;
    };
    template<typename T>
    struct is_vector2_type<vector2<T>> {const static bool value = true;};


    /// int_type used to determine what boolean operators should return

    template<typename T>
    using int_type = typename T::int_type;

    /// Operators

    #define MAKE_UNARY_OPERATOR(RETURN_TYPE, TYPE, OP, ENUM)\
    inline\
    __attribute__((always_inline))\
    const rvalue<RETURN_TYPE> operator OP(const rvalue<TYPE>& right)\
    {\
        const auto dt = RETURN_TYPE::type;\
        const auto op = Operator::ENUM;\
        return rvalue<RETURN_TYPE>(spark_create_operator1_node(dt, op, right._node));\
    }

    #define MAKE_BINARY_OPERATOR(RETURN_TYPE, TYPE, OP, ENUM)\
    inline\
    __attribute__((always_inline))\
    const rvalue<RETURN_TYPE> operator OP (const rvalue<TYPE>& left, const rvalue<TYPE>& right)\
    {\
        const auto dt = RETURN_TYPE::type;\
        const auto op = Operator::ENUM;\
        return rvalue<RETURN_TYPE>(spark_create_operator2_node(dt, op, left._node, right._node));\
    }

    #define MAKE_PREFIX_OPERATOR(RETURN_TYPE, TYPE, OP, ENUM)\
    inline\
    __attribute__((always_inline))\
    const rvalue<RETURN_TYPE, true> operator OP (const TYPE& value)\
    {\
        const auto dt = RETURN_TYPE::type;\
        const auto op = Operator::ENUM;\
        return rvalue<RETURN_TYPE, true>(spark_create_operator1_node(dt, op, value._node));\
    }

    #define MAKE_POSTFIX_OPERATOR(RETURN_TYPE, TYPE, OP, ENUM)\
    inline\
    __attribute__((always_inline))\
    const rvalue<RETURN_TYPE, true> operator OP (const TYPE& value, int)\
    {\
        const auto dt = RETURN_TYPE::type;\
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
    MAKE_POSTFIX_OPERATOR(TYPE, TYPE, --, PostfixDecrement)\

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
    MAKE_BINARY_OPERATOR(TYPE, TYPE, <<, LeftShift)\

    #define MAKE_TYPEDEFS(TYPE, RAW_TYPE)\
    typedef scalar<RAW_TYPE> TYPE;\
    typedef vector2<scalar<RAW_TYPE>> TYPE##2;\
    typedef Pointer<TYPE> P##TYPE;\
    typedef Pointer<TYPE##2> P##TYPE##2;\

    #define MAKE_STATICS(TYPE)\
    template<> const char* TYPE::name = #TYPE;\
    template<> const char* TYPE##2::name = #TYPE "2";\
    template<> const char* P##TYPE::name = "Pointer<" #TYPE ">";\
    template<> const char* P##TYPE##2::name = "Pointer<" #TYPE "2>";\
    template<> const datatype_t TYPE::type = DataType::TYPE;\
    template<> const datatype_t TYPE##2::type = (datatype_t)(DataType::TYPE | DataType::Vector2);\
    template<> const datatype_t P##TYPE::type = (datatype_t)(DataType::TYPE | DataType::Pointer);\
    template<> const datatype_t P##TYPE##2::type = (datatype_t)(DataType::TYPE | DataType::Vector2 | DataType::Pointer);\

    #define MAKE_INT_TYPES(TYPE, CL_TYPE)\
    MAKE_TYPEDEFS(TYPE, CL_TYPE)\
    MAKE_STATICS(TYPE)\
    MAKE_INT_OPERATORS(TYPE)\
    MAKE_INT_OPERATORS(TYPE##2)\

    #define MAKE_FLOAT_TYPES(TYPE, CL_TYPE)\
    MAKE_TYPEDEFS(TYPE, CL_TYPE)\
    MAKE_STATICS(TYPE)\
    MAKE_FLOAT_OPERATORS(TYPE)\
    MAKE_FLOAT_OPERATORS(TYPE##2)\

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

    // void type
    typedef void Void;

    // pointer operators
    template<typename TYPE>
    inline
    __attribute__((always_inline))
    const rvalue<Pointer<TYPE>> operator&(const scalar<TYPE>& right)\
    {\
        const auto dt = (datatype_t)(TYPE::type | DataType::Pointer);
        const auto op = Operator::AddressOf;\
        return rvalue<Pointer<TYPE>>(spark_create_operator1_node(dt, op, right._node));\
    }

    template<typename TYPE>
    inline
    __attribute__((always_inline))
    const rvalue<Pointer<TYPE>> operator&(const vector2<TYPE>& right)\
    {\
        const auto dt = (datatype_t)(TYPE::type | DataType::Pointer);
        const auto op = Operator::AddressOf;\
        return rvalue<Pointer<TYPE>>(spark_create_operator1_node(dt, op, right._node));\
    }

    template<typename TYPE>
    inline
    __attribute__((always_inline))
    const rvalue<Pointer<TYPE>> operator+(const Pointer<TYPE>& pointer, const rvalue<UInt>& offset)
    {
        const auto dt = (datatype_t)(TYPE::type | DataType::Pointer);
        const auto op = Operator::Add;
        return rvalue<Pointer<TYPE>>(spark_create_operator2_node(dt, op, pointer._node, offset._node));
    }

    template<typename TYPE>
    inline
    __attribute__((always_inline))
    const rvalue<Pointer<TYPE>> operator+(const rvalue<UInt>& offset, const Pointer<TYPE>& pointer)
    {
        const auto dt = (datatype_t)(TYPE::type | DataType::Pointer);
        const auto op = Operator::Add;
        return rvalue<Pointer<TYPE>>(spark_create_operator2_node(dt, op, offset._node, pointer._node));
    }

}