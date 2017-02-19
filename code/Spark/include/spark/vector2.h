#pragma once

namespace Spark
{
    namespace Internal
    {
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
    }
}