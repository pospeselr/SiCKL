#pragma once

namespace spark
{
    // forward declare tyeps

    namespace client
    {
        template<typename HOST_TYPE> struct scalar;
        template<typename TYPE> struct device_vector2;
        template<typename TYPE> struct device_vector4;
        template<typename TYPE> struct pointer;

        inline
        SPARK_NEVER_INLINE
        void destruct_attach(spark_node_t* node)
        {
            bool nodeAttached = spark_node_get_attached(node, THROW_ON_ERROR());
            if(nodeAttached == false)
            {
                // add to tree
                spark_node_t* currentScope = spark_peek_scope_node(THROW_ON_ERROR());
                spark_add_child_node(currentScope, node, THROW_ON_ERROR());
            }
        }

        template<typename TYPE, bool DESTRUCT_ATTACH=false> struct rvalue;

        // scalar rvalue
        template<typename HOST_TYPE, bool DESTRUCT_ATTACH>
        struct rvalue<scalar<HOST_TYPE>, DESTRUCT_ATTACH> : scalar<HOST_TYPE>
        {
            typedef scalar<HOST_TYPE> TYPE;

            SPARK_FORCE_INLINE
            rvalue(spark_node_t* node) : TYPE(node) {}

            SPARK_FORCE_INLINE
            rvalue(const TYPE& that) : rvalue(that._node) {}

            SPARK_FORCE_INLINE
            rvalue(HOST_TYPE val) : rvalue(spark_create_constant_node(static_cast<spark_datatype_t>(TYPE::type), &val, sizeof(val), THROW_ON_ERROR())) {}

            // constructor explicitly takes an int, and converts to HOST_TYPE
            // reolves abiguity with other types
            template<typename PRIMITIVE = HOST_TYPE>
            rvalue(typename is_int<PRIMITIVE>::type val) : rvalue(static_cast<HOST_TYPE>(val)) {}

            SPARK_FORCE_INLINE
            ~rvalue()
            {
                // used for prefix/postix operator (or any operator which has side effects and
                // also returns an rvalue which may be ignored)

                // from 12.2/3:

                // Temporary objects are destroyed as the last step in evaluating the full-
                // expression (1.9) that (lexically) contains the point where they were created.

                if(DESTRUCT_ATTACH)
                {
                    destruct_attach(this->_node);
                }
            }
            rvalue& operator=(const rvalue&) = delete;
        };

        // device_vector2 rvalue
        template<typename SCALAR_TYPE, bool DESTRUCT_ATTACH>
        struct rvalue<device_vector2<SCALAR_TYPE>, DESTRUCT_ATTACH> : device_vector2<SCALAR_TYPE>
        {
            typedef device_vector2<SCALAR_TYPE> TYPE;
            typedef typename SCALAR_TYPE::host_type HOST_TYPE;

            rvalue(spark_node_t* node) : TYPE(node) {}
            rvalue(const TYPE& that) : rvalue(that._node) {}
            rvalue(const rvalue<SCALAR_TYPE>& x, const rvalue<SCALAR_TYPE>& y)
            : rvalue(
                [](spark_node_t* x, spark_node_t* y) -> spark_node_t*
                {
                    spark_node_t* children[] = {x, y};
                    auto listNode = spark_create_vector_node(static_cast<spark_datatype_t>(TYPE::type), children, countof(children), THROW_ON_ERROR());
                    return listNode;
                }(x._node, y._node))
            {

            }

            SPARK_FORCE_INLINE
            ~rvalue()
            {
                // used for prefix/postix operator (or any operator which has side effects and
                // also returns an rvalue which may be ignored)

                // from 12.2/3:

                // Temporary objects are destroyed as the last step in evaluating the full-
                // expression (1.9) that (lexically) contains the point where they were created.

                if(DESTRUCT_ATTACH)
                {
                    destruct_attach(this->_node);
                }
            }
            rvalue& operator=(const rvalue&) = delete;
        };

        // device_vector2 rvalue
        template<typename SCALAR_TYPE, bool DESTRUCT_ATTACH>
        struct rvalue<device_vector4<SCALAR_TYPE>, DESTRUCT_ATTACH> : device_vector4<SCALAR_TYPE>
        {
            typedef device_vector4<SCALAR_TYPE> TYPE;
            typedef typename SCALAR_TYPE::host_type HOST_TYPE;

            rvalue(spark_node_t* node) : TYPE(node) {}
            rvalue(const TYPE& that) : rvalue(that._node) {}
            rvalue(const rvalue<SCALAR_TYPE>& x, const rvalue<SCALAR_TYPE>& y, const rvalue<SCALAR_TYPE>& z, const rvalue<SCALAR_TYPE>& w)
            : rvalue(
                [](spark_node_t* x, spark_node_t* y, spark_node_t* z, spark_node_t* w) -> spark_node_t*
                {
                    spark_node_t* children[] = {x, y, z, w};
                    auto listNode = spark_create_vector_node(static_cast<spark_datatype_t>(TYPE::type), children, countof(children), THROW_ON_ERROR());
                    return listNode;
                }(x._node, y._node, z._node, w._node))
            {

            }

            SPARK_FORCE_INLINE
            ~rvalue()
            {
                // used for prefix/postix operator (or any operator which has side effects and
                // also returns an rvalue which may be ignored)

                // from 12.2/3:

                // Temporary objects are destroyed as the last step in evaluating the full-
                // expression (1.9) that (lexically) contains the point where they were created.

                if(DESTRUCT_ATTACH)
                {
                    destruct_attach(this->_node);
                }
            }
            rvalue& operator=(const rvalue&) = delete;
        };

        // pointer rvalue
        template<typename BASE_TYPE, bool DESTRUCT_ATTACH>
        struct rvalue<pointer<BASE_TYPE>,DESTRUCT_ATTACH> : pointer<BASE_TYPE>
        {
            typedef pointer<BASE_TYPE> TYPE;

            rvalue(spark_node_t* node) : TYPE(node) {}
            rvalue(const TYPE& that) : rvalue(that._node) {}

            SPARK_FORCE_INLINE
            ~rvalue()
            {
                // used for prefix/postix operator (or any operator which has side effects and
                // also returns an rvalue which may be ignored)

                // from 12.2/3:

                // Temporary objects are destroyed as the last step in evaluating the full-
                // expression (1.9) that (lexically) contains the point where they were created.

                if(DESTRUCT_ATTACH)
                {
                    destruct_attach(this->_node);
                }
            }
            rvalue& operator=(const rvalue&) = delete;
        };
    }
}