#pragma once

namespace Spark
{
    namespace Internal
    {
        inline
        __attribute__ ((noinline))
        spark_node_t* extern_constructor(datatype_t dt)
        {
            auto thisNode = spark_create_symbol_node(dt, Spark::Internal::ThrowOnError());
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

            auto thisNode = spark_create_symbol_node(dt, Spark::Internal::ThrowOnError());

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

            auto thisNode = spark_create_symbol_node(dt, Spark::Internal::ThrowOnError());
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
}