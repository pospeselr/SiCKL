#pragma once

namespace Spark
{
    template<typename TYPE, bool DESTRUCT_ATTACH=false>
    struct rvalue : TYPE
    {
        rvalue(const TYPE& that) : TYPE(that._node) {}
        rvalue(Node* node) : TYPE(node) {}
        rvalue(nullptr_t) : TYPE(nullptr) {}
        ~rvalue()
        {
            // used for prefix/postix operator (or any operator which has side effects and
            // also returns an rvalue which may be ignored)

            // from 12.2/3:

            // Temporary objects are destroyed as the last step in evaluating the full-
            // expression (1.9) that (lexically) contains the point where they were created.

            if(DESTRUCT_ATTACH &&
               this->_node->_attached == false)
            {
                // add to tree
                Node* currentScope = spark_peek_scope_node();
                spark_add_child_node(currentScope, this->_node);
            }
        }
        rvalue& operator=(const rvalue&) = delete;
        rvalue(typename TYPE::cl_type raw) : TYPE(spark_create_constant_node(TYPE::type, &raw, sizeof(raw))) {}
    };
}