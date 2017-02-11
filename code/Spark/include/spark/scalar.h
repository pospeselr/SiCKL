#pragma once

namespace Spark
{
    namespace Internal
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
            template<typename S, Spark::Internal::Property id>
            friend struct property_rw;
            template<typename S>
            friend struct pointer;
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
    }
}