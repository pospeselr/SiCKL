#pragma once

namespace Spark
{
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

    template<typename TYPE, bool DESTRUCT_ATTACH=false>
    struct rvalue : TYPE
    {
        rvalue(const TYPE& that) : TYPE(that._node) {}
        rvalue(Node* node) : TYPE(node) {}
        rvalue(nullptr_t) : TYPE(nullptr) {}
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
        inline
        __attribute__ ((always_inline))
        rvalue(typename TYPE::cl_type raw) : TYPE(spark_create_constant_node(TYPE::type, &raw, sizeof(raw), Spark::Internal::ThrowOnError())) {}
    };
}