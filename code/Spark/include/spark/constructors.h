#pragma once

namespace Spark
{
    namespace Internal
    {
        inline
        __attribute__ ((noinline))
        Node* extern_constructor(datatype_t dt)
        {
            const auto id = spark_next_symbol(Spark::Internal::ThrowOnError());
            Node* thisNode = spark_create_symbol_node(dt, id, Spark::Internal::ThrowOnError());

            return thisNode;
        }

        inline
        __attribute__ ((noinline))
        Node* value_constructor(datatype_t dt, const void* raw, size_t sz)
        {
            const auto op = Operator::Assignment;
            const auto id = spark_next_symbol(Spark::Internal::ThrowOnError());

            Node* thisNode = spark_create_symbol_node(dt, id, Spark::Internal::ThrowOnError());

            // init to val
            Node* valNode = spark_create_constant_node(dt, raw, sz, Spark::Internal::ThrowOnError());

            // create assignment node
            Node* assignmentNode = spark_create_operator2_node(dt, op, thisNode, valNode);
            // add to tree
            Node* currentScope = spark_peek_scope_node(Spark::Internal::ThrowOnError());
            spark_add_child_node(currentScope, assignmentNode, Spark::Internal::ThrowOnError());

            return thisNode;
        }

        inline
        __attribute__ ((noinline))
        Node* copy_constructor(datatype_t dt, Node* that)
        {
            const auto op = Operator::Assignment;
            const auto id = spark_next_symbol(Spark::Internal::ThrowOnError());

            Node* thisNode = spark_create_symbol_node(dt, id, Spark::Internal::ThrowOnError());
            Node* thatNode = that;

            // create assignment node
            Node* assignmentNode = spark_create_operator2_node(dt, op, thisNode, thatNode);
            // add to tree
            Node* currentScope = spark_peek_scope_node(Spark::Internal::ThrowOnError());
            spark_add_child_node(currentScope, assignmentNode, Spark::Internal::ThrowOnError());

            return thisNode;
        }

        inline
        __attribute__ ((noinline))
        Node* list_constructor(datatype_t dt, Node** children, size_t count)
        {
            Node* listNode = spark_create_list_node(Spark::Internal::ThrowOnError());
            for(size_t k = 0; k < count; k++)
            {
                spark_add_child_node(listNode, children[k], Spark::Internal::ThrowOnError());
            }

            Node* thisNode = copy_constructor(dt, listNode);
            return thisNode;
        }

    }

    // variable declaration 'constructor' (used for symbols passed into functions with no defined value)
    template<typename TYPE>
    void extern_constructor(TYPE* pThis)
    {
        pThis->_node = Internal::extern_constructor(TYPE::type);
    }

    // value constructor
    template<typename TYPE, size_t SIZE>
    void value_constructor(TYPE* pThis, const void* raw)
    {
        pThis->_node = Internal::value_constructor(TYPE::type, raw, SIZE);
    }

    // default constructor (ie 0)
    template<typename TYPE, typename CL_TYPE>
    void default_constructor(TYPE* pThis)
    {
        // init to 0
        CL_TYPE val = {0};
        value_constructor<TYPE, sizeof(val)>(pThis, &val);
    }

    // copy constructor
    template<typename TYPE>
    void copy_constructor(TYPE* pThis, const TYPE& that)
    {
        pThis->_node = Internal::copy_constructor(TYPE::type, that._node);
    }
}