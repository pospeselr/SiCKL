#pragma once

namespace spark
{
    namespace client
    {
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
            template<typename S, spark::shared::Property id>
            friend struct property_rw;
            template<typename S>
            friend struct pointer;
        protected:
            // node constructor
            inline
            SPARK_FORCE_INLINE
            scalar(spark_node_t* node)
            : _node(node)
            {
                SPARK_ASSERT(_node != nullptr);
            }
        public:
            // constructors
            inline
            SPARK_FORCE_INLINE
            scalar()
            {
                RAW_TYPE val = {};
                this->_node = value_constructor(scalar::type, &val, sizeof(val));
            }

            inline
            SPARK_FORCE_INLINE
            scalar(std::nullptr_t)
            {
                this->_node = extern_constructor(scalar::type);
            }

            inline
            SPARK_FORCE_INLINE
            scalar(RAW_TYPE val)
            {
                this->_node = value_constructor(scalar::type, &val, sizeof(val));
            }

            inline
            SPARK_FORCE_INLINE
            scalar(const scalar& that)
            {
                this->_node = copy_constructor(scalar::type, that._node);
            }

            inline
            SPARK_FORCE_INLINE
            scalar(const lvalue<scalar>& that)
            {
                this->_node = copy_constructor(scalar::type, that._node);
            }

            inline
            SPARK_FORCE_INLINE
            scalar(const rvalue<scalar>& that)
            {
                this->_node = copy_constructor(scalar::type, that._node);
            }

            scalar(scalar&&) = default;

            inline
            SPARK_FORCE_INLINE
            scalar& operator=(const scalar& that)
            {
                assignment_operator(this->_node, scalar::type, that._node);
                return *this;
            }

            inline
            SPARK_FORCE_INLINE
            scalar& operator=(RAW_TYPE val)
            {
                const auto dt = static_cast<spark_datatype_t>(scalar::type);
                auto constant = spark_create_constant_node(dt, &val, sizeof(val), ThrowOnError());
                assignment_operator(this->_node, scalar::type, constant);
                return *this;
            }

            // cast operator
            template<typename CAST_TYPE>
            const rvalue<CAST_TYPE> As() const
            {
                static_assert(is_scalar_type<CAST_TYPE>::value, "scalar types can only be cast to other scalar types");

                const auto dt = static_cast<spark_datatype_t>(CAST_TYPE::type);
                const auto op = static_cast<spark_operator_t>(spark::shared::Operator::Cast);
                return rvalue<CAST_TYPE>(spark_create_operator1_node(dt, op, this->_node));
            }

            // private node ptr
            spark_node_t* _node;
            static constexpr spark::shared::Datatype type = spark::shared::Datatype(type_to_primitive<RAW_TYPE>::value, spark::shared::Components::Scalar, false);
        };
        template<typename T>
        constexpr spark::shared::Datatype scalar<T>::type;
        template<typename T> struct is_scalar_type<scalar<T>> {const static bool value = true;};
    }
}