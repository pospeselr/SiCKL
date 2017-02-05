#pragma once

namespace Spark
{
    using namespace Internal;

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
}