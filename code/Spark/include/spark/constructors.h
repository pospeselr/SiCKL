#pragma once

namespace Spark
{
    // variable declaration 'constructor' (used for symbols passed into functions with no defined value)
    template<typename TYPE>
    void extern_constructor(TYPE* pThis)
    {
        const auto dt = TYPE::type;

        Node* thisNode = spark_create_symbol_node(dt, spark_next_symbol());
        pThis->_node = thisNode;
    }

    // value constructor
    template<typename TYPE, size_t SIZE>
    void value_constructor(TYPE* pThis, const void* raw)
    {
        const auto dt = TYPE::type;
        const auto op = Operator::Assignment;

        Node* thisNode = spark_create_symbol_node(dt, spark_next_symbol());
        pThis->_node = thisNode;

        // init to val
        Node* valNode = spark_create_constant_node(dt, raw, SIZE);

        // create assignment node
        Node* assignmentNode = spark_create_operator2_node(dt, op, thisNode, valNode);
        // add to tree
        Node* currentScope = spark_peek_scope_node();
        spark_add_child_node(currentScope, assignmentNode);
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
        const auto dt = TYPE::type;
        const auto op = Operator::Assignment;

        Node* thisNode = spark_create_symbol_node(dt, spark_next_symbol());
        pThis->_node = thisNode;

        Node* thatNode = that._node;

        // create assignment node
        Node* assignmentNode = spark_create_operator2_node(dt, op, thisNode, thatNode);
        // add to tree
        Node* currentScope = spark_peek_scope_node();
        spark_add_child_node(currentScope, assignmentNode);
    }
}