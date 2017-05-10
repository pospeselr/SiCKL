#pragma once

namespace spark
{
    namespace client
    {
        template<typename TYPE>
        struct host_vector2
        {
            union
            {
                struct
                {
                    TYPE x;
                    TYPE y;
                };
                TYPE data[2];
            };
        };
    }
    typedef client::host_vector2<int8_t> char2;
    typedef client::host_vector2<uint8_t> uchar2;
    typedef client::host_vector2<int16_t> short2;
    typedef client::host_vector2<uint16_t> ushort2;
    typedef client::host_vector2<int32_t> int2;
    typedef client::host_vector2<uint32_t> uint2;
    typedef client::host_vector2<int64_t> long2;
    typedef client::host_vector2<uint64_t> ulong2;
    typedef client::host_vector2<float> float2;
    typedef client::host_vector2<double> double2;

    namespace client
    {
        // wrapper for all device_vector2 types
        template<typename TYPE>
        struct device_vector2
        {
        public:
            // typedefs
            typedef device_vector2<scalar<typename type_to_signed_int<typename TYPE::host_type>::type>> int_type;
            typedef host_vector2<typename TYPE::host_type> host_type;

            // friends
            template<typename S, spark::shared::Property id>
            friend struct property_rw;
        protected:
            // node constructor
            SPARK_FORCE_INLINE
            device_vector2(spark_node_t* node)
            : _node(node)
            {
                SPARK_ASSERT(_node != nullptr);
            }
        public:
            // constructors
            SPARK_FORCE_INLINE
            device_vector2() : device_vector2(0.0f, 0.0f) {}

            SPARK_FORCE_INLINE
            device_vector2(null_construct_t) : _node(nullptr) {}

            SPARK_FORCE_INLINE
            device_vector2(extern_construct_t)
            {
                this->_node = extern_constructor(device_vector2::type);
            }

            SPARK_FORCE_INLINE
            device_vector2(const rvalue<device_vector2>& that)
            {
                this->_node = copy_constructor(device_vector2::type, that._node);
            }

            SPARK_FORCE_INLINE
            device_vector2(const lvalue<device_vector2>& that)
            {
                this->_node = copy_constructor(device_vector2::type, that._node);
            }


            SPARK_FORCE_INLINE
            device_vector2(const device_vector2& that)
            {
                this->_node = copy_constructor(device_vector2::type, that._node);
            }

            SPARK_FORCE_INLINE
            device_vector2(const rvalue<TYPE>& x, const rvalue<TYPE>& y)
            {
                this->_node = vector_constructor(device_vector2::type, {x._node, y._node});
            }

            device_vector2(device_vector2&&) = default;

            SPARK_FORCE_INLINE
            device_vector2& operator=(const device_vector2& that)
            {
                assignment_operator(this->_node, device_vector2::type, that._node);
                return *this;
            }

            // properties
            union
            {
                // swizzles
                property_rw<TYPE, spark::shared::Property::X> X;
                property_rw<TYPE, spark::shared::Property::Y> Y;
                property_r<device_vector2, spark::shared::Property::XX> XX;
                property_rw<device_vector2, spark::shared::Property::XY> XY;
                property_rw<device_vector2, spark::shared::Property::YX> YX;
                property_r<device_vector2, spark::shared::Property::YY> YY;
                // others
                property_rw<TYPE, spark::shared::Property::Lo> Lo;
                property_rw<TYPE, spark::shared::Property::Hi> Hi;
                property_rw<TYPE, spark::shared::Property::Even> Even;
                property_rw<TYPE, spark::shared::Property::Odd> Odd;

                // private node ptr
                spark_node_t* _node;
            };

            // cast operator
            template<typename CAST_TYPE>
            const rvalue<CAST_TYPE> As() const
            {
                static_assert(is_vector2_type<CAST_TYPE>::value, "vector2 types can only be cast to other device_vector2 types");

                const auto dt = static_cast<spark_datatype_t>(TYPE::type);
                const auto op = static_cast<spark_operator_t>(spark::shared::Operator::Cast);

                return rvalue<CAST_TYPE>(spark_create_operator1_node(dt, op, this->_node));
            }

            static constexpr spark::shared::Datatype type = spark::shared::Datatype(TYPE::type.GetPrimitive(), spark::shared::Components::Vector2, false);
        };
        template<typename T>
        constexpr spark::shared::Datatype device_vector2<T>::type;
        template<typename T>
        struct is_vector2_type<device_vector2<T>> {const static bool value = true;};
    }
}