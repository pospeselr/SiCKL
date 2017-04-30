#pragma once

namespace spark
{
    namespace client
    {
        template<typename TYPE>
        struct host_vector4
        {
            union
            {
                struct
                {
                    TYPE x;
                    TYPE y;
                    TYPE z;
                    TYPE w;
                };
                TYPE data[4];
            };
        };
    }
    typedef client::host_vector4<int8_t> char4;
    typedef client::host_vector4<uint8_t> uchar4;
    typedef client::host_vector4<int16_t> short4;
    typedef client::host_vector4<uint16_t> ushort4;
    typedef client::host_vector4<int32_t> int4;
    typedef client::host_vector4<uint32_t> uint4;
    typedef client::host_vector4<int64_t> long4;
    typedef client::host_vector4<uint64_t> ulong4;
    typedef client::host_vector4<float> float4;
    typedef client::host_vector4<double> double4;

    namespace client
    {
        // wrapper for all device_vector4 types
        template<typename TYPE>
        struct device_vector4
        {
        public:
            // typedefs
            typedef device_vector4<scalar<typename type_to_signed_int<typename TYPE::host_type>::type>> int_type;
            typedef host_vector4<typename TYPE::host_type> host_type;

            // friends
            template<typename S, spark::shared::Property id>
            friend struct property_rw;
        protected:
            // node constructor
            SPARK_FORCE_INLINE
            device_vector4(spark_node_t* node)
            : _node(node)
            {
                SPARK_ASSERT(_node != nullptr);
            }
        public:
            // constructors
            SPARK_FORCE_INLINE
            device_vector4() : device_vector4(0.0f, 0.0f, 0.0f, 0.0f) {}

            SPARK_FORCE_INLINE
            device_vector4(null_construct_t) : _node(nullptr) {}

            SPARK_FORCE_INLINE
            device_vector4(extern_construct_t)
            {
                this->_node = extern_constructor(device_vector4::type);
            }

            SPARK_FORCE_INLINE
            device_vector4(const rvalue<device_vector4>& that)
            {
                this->_node = copy_constructor(device_vector4::type, that._node);
            }

            SPARK_FORCE_INLINE
            device_vector4(const lvalue<device_vector4>& that)
            {
                this->_node = copy_constructor(device_vector4::type, that._node);
            }

            SPARK_FORCE_INLINE
            device_vector4(const device_vector4& that)
            {
                this->_node = copy_constructor(device_vector4::type, that._node);
            }

            SPARK_FORCE_INLINE
            device_vector4(const rvalue<TYPE>& x, const rvalue<TYPE>& y, const rvalue<TYPE>& z, const rvalue<TYPE>& w)
            {
                this->_node = vector_constructor(device_vector4::type, {x._node, y._node, z._node, w._node});
            }

            device_vector4(device_vector4&&) = default;

            SPARK_FORCE_INLINE
            device_vector4& operator=(const device_vector4& that)
            {
                assignment_operator(this->_node, device_vector4::type, that._node);
                return *this;
            }

            SPARK_FORCE_INLINE
            device_vector4& operator=(device_vector4&& that)
            {
                this->_node = that._node;
                return *this;
            }

            // properties
            union
            {
                // swizzles
                property_rw<TYPE, spark::shared::Property::X> X;
                property_rw<TYPE, spark::shared::Property::Y> Y;
                property_rw<TYPE, spark::shared::Property::Z> Z;
                property_rw<TYPE, spark::shared::Property::W> W;

                // others
                property_rw<device_vector2<TYPE>, spark::shared::Property::Lo> Lo;
                property_rw<device_vector2<TYPE>, spark::shared::Property::Hi> Hi;
                property_rw<device_vector2<TYPE>, spark::shared::Property::Even> Even;
                property_rw<device_vector2<TYPE>, spark::shared::Property::Odd> Odd;

                // private node ptr
                spark_node_t* _node;
            };

            // cast operator
            template<typename CAST_TYPE>
            const rvalue<CAST_TYPE> As() const
            {
                static_assert(is_vector4_type<CAST_TYPE>::value, "vector4 types can only be cast to other device_vector4 types");

                const auto dt = static_cast<spark_datatype_t>(TYPE::type);
                const auto op = static_cast<spark_operator_t>(spark::shared::Operator::Cast);

                return rvalue<CAST_TYPE>(spark_create_operator1_node(dt, op, this->_node));
            }

            static constexpr spark::shared::Datatype type = spark::shared::Datatype(TYPE::type.GetPrimitive(), spark::shared::Components::Vector4, false);
        };
        template<typename T>
        constexpr spark::shared::Datatype device_vector4<T>::type;
        template<typename T>
        struct is_vector4_type<device_vector4<T>> {const static bool value = true;};
    }
}