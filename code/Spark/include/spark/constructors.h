#pragma once

namespace Spark
{
    namespace Internal
    {
        inline
        __attribute__ ((noinline))
        spark_node_t* extern_constructor(datatype_t dt)
        {
            const auto id = spark_next_symbol(Spark::Internal::ThrowOnError());
            auto thisNode = spark_create_symbol_node(dt, id, Spark::Internal::ThrowOnError());

            return thisNode;
        }

        inline
        __attribute__ ((noinline))
        spark_node_t* value_constructor(datatype_t dt, const void* raw, size_t sz)
        {
            SPARK_ASSERT(dt == DataType::Char ||
                         dt == DataType::UChar ||
                         dt == DataType::Short ||
                         dt == DataType::UShort ||
                         dt == DataType::Int ||
                         dt == DataType::UInt ||
                         dt == DataType::Long ||
                         dt == DataType::ULong ||
                         dt == DataType::Float ||
                         dt == DataType::Double);

            const auto op = Operator::Assignment;
            const auto id = spark_next_symbol(Spark::Internal::ThrowOnError());

            auto thisNode = spark_create_symbol_node(dt, id, Spark::Internal::ThrowOnError());

            // init to val
            auto valNode = spark_create_constant_node(dt, raw, sz, Spark::Internal::ThrowOnError());

            // create assignment node
            auto assignmentNode = spark_create_operator2_node(dt, op, thisNode, valNode);
            // add to tree
            auto currentScope = spark_peek_scope_node(Spark::Internal::ThrowOnError());
            spark_add_child_node(currentScope, assignmentNode, Spark::Internal::ThrowOnError());

            return thisNode;
        }

        inline
        __attribute__ ((noinline))
        spark_node_t* copy_constructor(datatype_t dt, spark_node_t* that)
        {
            const auto op = Operator::Assignment;
            const auto id = spark_next_symbol(Spark::Internal::ThrowOnError());

            auto thisNode = spark_create_symbol_node(dt, id, Spark::Internal::ThrowOnError());
            auto thatNode = that;

            // create assignment node
            auto assignmentNode = spark_create_operator2_node(dt, op, thisNode, thatNode);
            // add to tree
            auto currentScope = spark_peek_scope_node(Spark::Internal::ThrowOnError());
            spark_add_child_node(currentScope, assignmentNode, Spark::Internal::ThrowOnError());

            return thisNode;
        }

        inline
        __attribute__ ((noinline))
        spark_node_t* vector_constructor(datatype_t dt, spark_node_t** children, size_t count)
        {
            auto listNode = spark_create_vector_node(dt, children, count, Spark::Internal::ThrowOnError());
            auto thisNode = copy_constructor(dt, listNode);
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