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
                return lvalue<TYPE>(dereference(this->_node));
            }

            SPARK_FORCE_INLINE
            rvalue<TYPE> operator*() const
            {
                return rvalue<TYPE>(dereference(this->_node));
            }

            SPARK_FORCE_INLINE
            lvalue<TYPE> operator[](const rvalue<scalar<int32_t>>& index)
            {
                const auto dt = static_cast<spark_datatype_t>(spark::shared::Datatype(TYPE::type.GetPrimitive(), TYPE::type.GetComponents(), true));
                const auto op = static_cast<spark_operator_t>(spark::shared::Operator::Add);
                spark_node_t* ptr_node = spark_create_operator2_node(dt, op, this->_node, index._node);
                return lvalue<TYPE>(dereference(ptr_node));
            }

            SPARK_FORCE_INLINE
            rvalue<TYPE> operator[](const rvalue<scalar<int32_t>>& index) const
            {
                const auto dt = static_cast<spark_datatype_t>(spark::shared::Datatype(TYPE::type.GetPrimitive(), TYPE::type.GetComponents(), true));
                const auto op = static_cast<spark_operator_t>(spark::shared::Operator::Add);
                spark_node_t* ptr_node = spark_create_operator2_node(dt, op, this->_node, index._node);
                return rvalue<TYPE>(dereference(ptr_node));
            }

            spark_node_t* _node = nullptr;
            friend TYPE;

            static const char* name;
            static constexpr spark::shared::Datatype type = spark::shared::Datatype(TYPE::type.GetPrimitive(), TYPE::type.GetComponents(), true);
        private:
            static spark_node_t* dereference(spark_node_t* ptr_node)
            {
                const auto dt = static_cast<spark_datatype_t>(TYPE::type);
                const auto op = static_cast<spark_operator_t>(spark::shared::Operator::Dereference);
                return spark_create_operator1_node(dt, op, ptr_node);
            }
        };
        template<typename TYPE>
        constexpr spark::shared::Datatype pointer<TYPE>::type;
    }
    template<class T>
    using Pointer = client::pointer<T>;
}