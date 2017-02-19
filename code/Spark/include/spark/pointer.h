#pragma once

namespace spark
{
    namespace client
    {
        // pointer type
        template<typename TYPE>
        struct pointer
        {
        protected:
            // node constructor
            pointer(spark_node_t* node)
            : _node(node)
            {
                SPARK_ASSERT(_node != nullptr);
            }
        public:
            // constructors
            inline
            __attribute__ ((always_inline))
            pointer(std::nullptr_t)
            {
                this->_node = extern_constructor(pointer::type);
            }

            inline
            __attribute__ ((always_inline))
            pointer(const pointer& that)
            {
                this->_node = copy_constructor(pointer::type, that._node);
            }

            inline
            __attribute__ ((always_inline))
            pointer(const rvalue<pointer>& that)
            {
                this->_node = copy_constructor(pointer::type, that._node);
            }

            pointer(pointer&&) = default;

            inline
            __attribute__ ((always_inline))
            pointer& operator=(const pointer& that)
            {
                const auto dt = static_cast<spark_datatype_t>(pointer::type);
                assignment_operator(this->_node, dt, that._node);
                return *this;
            }

            // pointer dereference operator
            inline
            __attribute__ ((always_inline))
            TYPE operator*()
            {
                const auto dt = static_cast<spark_datatype_t>(TYPE::type);
                const auto op = static_cast<spark_operator_t>(spark::shared::Operator::Dereference);
                return TYPE(spark_create_operator1_node(dt, op, this->_node));
            }

            inline
            __attribute__ ((always_inline))
            rvalue<TYPE> operator*() const
            {
                const auto dt = static_cast<spark_datatype_t>(TYPE::type);
                const auto op = static_cast<spark_operator_t>(spark::shared::Operator::Dereference);
                return rvalue<TYPE>(spark_create_operator1_node(dt, op, this->_node));
            }

            spark_node_t* _node = nullptr;
            friend TYPE;

            static const char* name;
            static constexpr spark::shared::Datatype type = spark::shared::Datatype(TYPE::type.GetPrimitive(), TYPE::type.GetComponents(), true);
        };
        template<typename TYPE>
        constexpr spark::shared::Datatype pointer<TYPE>::type;
    }
    template<class T>
    using Pointer = client::pointer<T>;
}