#pragma once

namespace spark
{
    namespace client
    {
        template<typename RAW_TYPE>
        struct scalar;

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
            SPARK_FORCE_INLINE
            pointer(extern_construct_t)
            {
                this->_node = extern_constructor(pointer::type);
            }

            SPARK_FORCE_INLINE
            pointer(const pointer& that)
            {
                this->_node = copy_constructor(pointer::type, that._node);
            }

            SPARK_FORCE_INLINE
            pointer(const rvalue<pointer>& that)
            {
                this->_node = copy_constructor(pointer::type, that._node);
            }

            pointer(pointer&&) = default;

            SPARK_FORCE_INLINE
            pointer& operator=(const pointer& that)
            {
                const auto dt = static_cast<spark_datatype_t>(pointer::type);
                assignment_operator(this->_node, dt, that._node);
                return *this;
            }

            // pointer dereference operator
            SPARK_FORCE_INLINE
            lvalue<TYPE> operator*()
            {
                const auto dt = static_cast<spark_datatype_t>(TYPE::type);
                const auto op = static_cast<spark_operator_t>(spark::shared::Operator::Dereference);
                return lvalue<TYPE>(spark_create_operator1_node(dt, op, this->_node));
            }

            SPARK_FORCE_INLINE
            rvalue<TYPE> operator*() const
            {
                const auto dt = static_cast<spark_datatype_t>(TYPE::type);
                const auto op = static_cast<spark_operator_t>(spark::shared::Operator::Dereference);
                return rvalue<TYPE>(spark_create_operator1_node(dt, op, this->_node));
            }

            SPARK_FORCE_INLINE
            lvalue<TYPE> operator[](const rvalue<scalar<int64_t>>& index)
            {
                const auto dt = static_cast<spark_datatype_t>(TYPE::type);
                const auto op = static_cast<spark_operator_t>(spark::shared::Operator::Index);
                return lvalue<TYPE>(spark_create_operator2_node(dt, op, this->_node, index._node));
            }

            SPARK_FORCE_INLINE
            rvalue<TYPE> operator[](const rvalue<scalar<int64_t>>& index) const
            {
                const auto dt = static_cast<spark_datatype_t>(TYPE::type);
                const auto op = static_cast<spark_operator_t>(spark::shared::Operator::Index);
                return rvalue<TYPE>(spark_create_operator2_node(dt, op, this->_node, index._node));
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