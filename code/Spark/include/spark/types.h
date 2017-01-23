#pragma once

namespace Spark
{
    namespace Internal
    {
        inline
        __attribute__ ((noinline))
        void assignment_operator(spark_node_t* thisNode, Datatype datatype, spark_node_t* thatNode)
        {
            const auto dt = static_cast<spark_datatype_t>(datatype);
            const auto op = static_cast<spark_operator_t>(Operator::Assignment);

            // create assignment node
            auto assignmentNode = spark_create_operator2_node(dt, op, thisNode, thatNode);
            // add to tree
            auto currentScope = spark_peek_scope_node(Spark::Internal::ThrowOnError());
            spark_add_child_node(currentScope, assignmentNode, Spark::Internal::ThrowOnError());
        }
    }
    using namespace Internal;

    /// Type properties

    template<typename T> struct is_scalar_type {const static bool value = false;};
    template<typename T> struct is_vector2_type {const static bool value = false;};

     /// Type Translations

    // gets the equivalent integer type (used for comparison operators)
    template<typename T> struct type_to_signed_int {typedef void type;};

#define MAKE_TYPE_TO_INT(TYPE, INT_TYPE)\
    template<> struct type_to_signed_int<TYPE> {typedef INT_TYPE type;};\

    MAKE_TYPE_TO_INT(int8_t, int8_t);
    MAKE_TYPE_TO_INT(uint8_t, int8_t);
    MAKE_TYPE_TO_INT(int16_t, int16_t);
    MAKE_TYPE_TO_INT(uint16_t, int16_t);
    MAKE_TYPE_TO_INT(int32_t, int32_t);
    MAKE_TYPE_TO_INT(uint32_t, int32_t);
    MAKE_TYPE_TO_INT(int64_t, int64_t);
    MAKE_TYPE_TO_INT(uint64_t, int64_t);
    MAKE_TYPE_TO_INT(float, int32_t);
    MAKE_TYPE_TO_INT(double, int64_t);

    template<typename T> struct type_to_primitive {};

#define MAKE_TYPE_TO_PRIMITIVE(TYPE, PRIMITIVE)\
    template<> struct type_to_primitive<TYPE> {static constexpr Primitive value = Internal::Primitive::PRIMITIVE;};

    MAKE_TYPE_TO_PRIMITIVE(int8_t, Char);
    MAKE_TYPE_TO_PRIMITIVE(uint8_t, UChar);
    MAKE_TYPE_TO_PRIMITIVE(int16_t, Short);
    MAKE_TYPE_TO_PRIMITIVE(uint16_t, UShort);
    MAKE_TYPE_TO_PRIMITIVE(int32_t, Int);
    MAKE_TYPE_TO_PRIMITIVE(uint32_t, UInt);
    MAKE_TYPE_TO_PRIMITIVE(int64_t, Long);
    MAKE_TYPE_TO_PRIMITIVE(uint64_t, ULong);
    MAKE_TYPE_TO_PRIMITIVE(float, Float);
    MAKE_TYPE_TO_PRIMITIVE(double, Double);

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

    // void type
    struct Void
    {
        static constexpr Datatype type = Datatype(Primitive::Void, Components::None, false);
    };
    constexpr Datatype Void::type;

    // forward declare tyeps
    template<typename RAW_TYPE> struct scalar;
    template<typename TYPE> struct vector2;

    // pointer type
    template<typename TYPE>
    struct Pointer
    {
    protected:
        // node constructor
        Pointer(spark_node_t* node)
        : _node(node)
        {
            SPARK_ASSERT(_node != nullptr);
        }
    public:
        // constructors
        inline
        __attribute__ ((always_inline))
        Pointer(std::nullptr_t)
        {
            this->_node = Internal::extern_constructor(Pointer::type);
        }

        inline
        __attribute__ ((always_inline))
        Pointer(const Pointer& that)
        {
            this->_node = Internal::copy_constructor(Pointer::type, that._node);
        }

        inline
        __attribute__ ((always_inline))
        Pointer(const rvalue<Pointer>& that)
        {
            this->_node = Internal::copy_constructor(Pointer::type, that._node);
        }

        Pointer(Pointer&&) = default;

        inline
        __attribute__ ((always_inline))
        Pointer& operator=(const Pointer& that)
        {
            const auto dt = static_cast<spark_datatype_t>(Pointer::type);
            Internal::assignment_operator(this->_node, dt, that._node);
            return *this;
        }

        // pointer dereference operator
        inline
        __attribute__ ((always_inline))
        TYPE operator*()
        {
            const auto dt = static_cast<spark_datatype_t>(TYPE::type);
            const auto op = static_cast<spark_operator_t>(Operator::Dereference);
            return TYPE(spark_create_operator1_node(dt, op, this->_node));
        }

        inline
        __attribute__ ((always_inline))
        rvalue<TYPE> operator*() const
        {
            const auto dt = static_cast<spark_datatype_t>(TYPE::type);
            const auto op = static_cast<spark_operator_t>(Operator::Dereference);
            return rvalue<TYPE>(spark_create_operator1_node(dt, op, this->_node));
        }

        spark_node_t* _node = nullptr;
        friend TYPE;

        static const char* name;
        static constexpr Datatype type = Datatype(TYPE::type.GetPrimitive(), TYPE::type.GetComponents(), true);
    };
    template<typename TYPE>
    constexpr Datatype Pointer<TYPE>::type;

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
        template<typename S, Spark::Internal::Property id>
        friend struct property_rw;
        template<typename S>
        friend struct Pointer;
    protected:
        // node constructor
        inline
        __attribute__ ((always_inline))
        scalar(spark_node_t* node)
        : _node(node)
        {
            SPARK_ASSERT(_node != nullptr);
        }
    public:
        // constructors
        inline
        __attribute__ ((always_inline))
        scalar()
        {
            RAW_TYPE val = {};
            this->_node = Internal::value_constructor(scalar::type, &val, sizeof(val));
        }

        inline
        __attribute__ ((always_inline))
        scalar(std::nullptr_t)
        {
            this->_node = Internal::extern_constructor(scalar::type);
        }

        inline
        __attribute__ ((always_inline))
        scalar(RAW_TYPE val)
        {
            this->_node = Internal::value_constructor(scalar::type, &val, sizeof(val));
        }

        inline
        __attribute__ ((always_inline))
        scalar(const scalar& that)
        {
            this->_node = Internal::copy_constructor(scalar::type, that._node);
        }

        inline
        __attribute__ ((always_inline))
        scalar(const lvalue<scalar>& that)
        {
            this->_node = Internal::copy_constructor(scalar::type, that._node);
        }

        inline
        __attribute__ ((always_inline))
        scalar(const rvalue<scalar>& that)
        {
            this->_node = Internal::copy_constructor(scalar::type, that._node);
        }

        scalar(scalar&&) = default;

        inline
        __attribute__ ((always_inline))
        scalar& operator=(const scalar& that)
        {
            Internal::assignment_operator(this->_node, scalar::type, that._node);
            return *this;
        }

        inline
        __attribute__ ((always_inline))
        scalar& operator=(RAW_TYPE val)
        {
            const auto dt = static_cast<spark_datatype_t>(scalar::type);
            auto constant = spark_create_constant_node(dt, &val, sizeof(val), Internal::ThrowOnError());
            Internal::assignment_operator(this->_node, scalar::type, constant);
            return *this;
        }

        // cast operator
        template<typename CAST_TYPE>
        const rvalue<CAST_TYPE> As() const
        {
            static_assert(is_scalar_type<CAST_TYPE>::value, "scalar types can only be cast to other scalar types");

            const auto dt = static_cast<spark_datatype_t>(CAST_TYPE::type);
            const auto op = static_cast<spark_operator_t>(Operator::Cast);
            return rvalue<CAST_TYPE>(spark_create_operator1_node(dt, op, this->_node));
        }

        // private node ptr
        spark_node_t* _node;
        static constexpr Datatype type = Datatype(type_to_primitive<RAW_TYPE>::value, Components::Scalar, false);
    };
    template<typename T>
    constexpr Datatype scalar<T>::type;
    template<typename T> struct is_scalar_type<scalar<T>> {const static bool value = true;};

    // wrapper for all vector2 types
    template<typename TYPE>
    struct vector2
    {
    public:
        // typedefs
        typedef vector2<scalar<typename type_to_signed_int<typename TYPE::raw_type>::type>> int_type;

        // friends
        template<typename S, Spark::Internal::Property id>
        friend struct property_rw;
    protected:
        // node constructor
        inline
        __attribute__ ((always_inline))
        vector2(spark_node_t* node)
        : _node(node)
        {
            SPARK_ASSERT(_node != nullptr);
        }
    public:
        // constructors
        inline
        __attribute__ ((always_inline))
        vector2() : vector2(0.0f, 0.0f) {}

        inline
        __attribute__ ((always_inline))
        vector2(std::nullptr_t)
        {
            this->_node = Internal::extern_constructor(vector2::type);
        }

        inline
        __attribute__ ((always_inline))
        vector2(const rvalue<vector2>& that)
        {
            this->_node = Internal::copy_constructor(vector2::type, that._node);
        }

        inline
        __attribute__ ((always_inline))
        vector2(const lvalue<vector2>& that)
        {
            this->_node = Internal::copy_constructor(vector2::type, that._node);
        }

        inline
        __attribute__ ((always_inline))
        vector2(const vector2& that)
        {
            this->_node = Internal::copy_constructor(vector2::type, that._node);
        }

        inline
        __attribute__ ((always_inline))
        vector2(const rvalue<TYPE>& x, const rvalue<TYPE>& y)
        {
            spark_node_t* children[] = {x._node, y._node};
            this->_node = Internal::vector_constructor(vector2::type, children, countof(children));
        }

        vector2(vector2&&) = default;

        inline
        __attribute__ ((always_inline))
        vector2& operator=(const vector2& that)
        {
            Internal::assignment_operator(this->_node, vector2::type, that._node);
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
            spark_node_t* _node;
        };

        // indexing operators
        inline
        __attribute__ ((always_inline))
        lvalue<TYPE> operator[](const rvalue<scalar<int32_t>>& index)
        {
            const auto dt = static_cast<spark_datatype_t>(TYPE::type);
            const auto op = static_cast<spark_operator_t>(Operator::Index);

            auto indexNode = spark_create_operator2_node(dt, op, this->_node, index._node);
            return lvalue<TYPE>(indexNode);
        }

        inline
        __attribute__ ((always_inline))
        const rvalue<TYPE> operator[](const rvalue<scalar<int32_t>>& index) const
        {
            const auto dt = static_cast<spark_datatype_t>(TYPE::type);
            const auto op = static_cast<spark_operator_t>(Operator::Index);

            auto indexNode = spark_create_operator2_node(dt, op, this->_node, index._node);
            return rvalue<TYPE>(indexNode);
        }

        // cast operator
        template<typename CAST_TYPE>
        const rvalue<CAST_TYPE> As() const
        {
            static_assert(is_vector2_type<CAST_TYPE>::value, "vector2 types can only be cast to other vector2 types");

            const auto dt = static_cast<spark_datatype_t>(TYPE::type);
            const auto op = static_cast<spark_operator_t>(Operator::Cast);

            return rvalue<CAST_TYPE>(spark_create_operator1_node(dt, op, this->_node));
        }

        static constexpr Datatype type = Datatype(TYPE::type.GetPrimitive(), Components::Vector2, false);
    };
    template<typename T>
    constexpr Datatype vector2<T>::type;
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