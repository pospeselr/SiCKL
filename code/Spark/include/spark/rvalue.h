#pragma once

namespace Spark
{
    // forward declare tyeps
    template<typename RAW_TYPE> struct scalar;
    template<typename TYPE> struct vector2;
    template<typename TYPE> struct Pointer;

    namespace Internal
    {
        inline
        __attribute__ ((noinline))
        void destruct_attach(Node* node)
        {
            bool nodeAttached = spark_node_get_attached(node, Spark::Internal::ThrowOnError());
            if(nodeAttached == false)
            {
                // add to tree
                Node* currentScope = spark_peek_scope_node(Spark::Internal::ThrowOnError());
                spark_add_child_node(currentScope, node, Spark::Internal::ThrowOnError());
            }
        }
    }

    template<typename TYPE, bool DESTRUCT_ATTACH=false> struct rvalue;

    // scalar rvalue
    template<typename RAW_TYPE, bool DESTRUCT_ATTACH>
    struct rvalue<scalar<RAW_TYPE>, DESTRUCT_ATTACH> : scalar<RAW_TYPE>
    {
        typedef scalar<RAW_TYPE> TYPE;

        rvalue(Node* node) : TYPE(node) {}
        rvalue(std::nullptr_t) : rvalue((Node*)nullptr) {}
        rvalue(const TYPE& that) : rvalue(that._node) {}
        rvalue(RAW_TYPE val) : rvalue(spark_create_constant_node(TYPE::type, &val, sizeof(val), Spark::Internal::ThrowOnError())) {}

        inline
        __attribute__ ((always_inline))
        ~rvalue()
        {
            // used for prefix/postix operator (or any operator which has side effects and
            // also returns an rvalue which may be ignored)

            // from 12.2/3:

            // Temporary objects are destroyed as the last step in evaluating the full-
            // expression (1.9) that (lexically) contains the point where they were created.

            if(DESTRUCT_ATTACH)
            {
                Spark::Internal::destruct_attach(this->_node);
            }
        }
        rvalue& operator=(const rvalue&) = delete;
    };

    // vector2 rvalue
    template<typename SCALAR_TYPE, bool DESTRUCT_ATTACH>
    struct rvalue<vector2<SCALAR_TYPE>, DESTRUCT_ATTACH> : vector2<SCALAR_TYPE>
    {
        typedef vector2<SCALAR_TYPE> TYPE;
        typedef typename SCALAR_TYPE::raw_type RAW_TYPE;

        rvalue(Node* node) : TYPE(node) {}
        rvalue(std::nullptr_t) : rvalue((Node*)nullptr) {}
        rvalue(const TYPE& that) : rvalue(that._node) {}
        rvalue(const RAW_TYPE (&val)[2]) : rvalue(spark_create_constant_node(TYPE::type, &val, sizeof(val), Spark::Internal::ThrowOnError())) {}

        inline
        __attribute__ ((always_inline))
        ~rvalue()
        {
            // used for prefix/postix operator (or any operator which has side effects and
            // also returns an rvalue which may be ignored)

            // from 12.2/3:

            // Temporary objects are destroyed as the last step in evaluating the full-
            // expression (1.9) that (lexically) contains the point where they were created.

            if(DESTRUCT_ATTACH)
            {
                Spark::Internal::destruct_attach(this->_node);
            }
        }
        rvalue& operator=(const rvalue&) = delete;
    };

    // pointer rvalue
    template<typename BASE_TYPE, bool DESTRUCT_ATTACH>
    struct rvalue<Pointer<BASE_TYPE>,DESTRUCT_ATTACH> : Pointer<BASE_TYPE>
    {
        typedef Pointer<BASE_TYPE> TYPE;

        rvalue(Node* node) : TYPE(node) {}
        rvalue(std::nullptr_t) : rvalue((Node*)nullptr) {}
        rvalue(const TYPE& that) : rvalue(that._node) {}

        inline
        __attribute__ ((always_inline))
        ~rvalue()
        {
            // used for prefix/postix operator (or any operator which has side effects and
            // also returns an rvalue which may be ignored)

            // from 12.2/3:

            // Temporary objects are destroyed as the last step in evaluating the full-
            // expression (1.9) that (lexically) contains the point where they were created.

            if(DESTRUCT_ATTACH)
            {
                Spark::Internal::destruct_attach(this->_node);
            }
        }
        rvalue& operator=(const rvalue&) = delete;
    };
}