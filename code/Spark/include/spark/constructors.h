#pragma once

namespace Spark
{
    namespace Internal
    {
        inline
        __attribute__ ((noinline))
        spark_node_t* extern_constructor(Datatype datatype)
        {
            const auto dt = static_cast<spark_datatype_t>(datatype);
            auto thisNode = spark_create_symbol_node(dt, Spark::Internal::ThrowOnError());
            return thisNode;
        }

        inline
        __attribute__ ((noinline))
        spark_node_t* value_constructor(Datatype datatype, const void* raw, size_t sz)
        {
            auto primitive = datatype.GetPrimitive();

            SPARK_ASSERT(primitive == Primitive::Char ||
                         primitive == Primitive::UChar ||
                         primitive == Primitive::Short ||
                         primitive == Primitive::UShort ||
                         primitive == Primitive::Int ||
                         primitive == Primitive::UInt ||
                         primitive == Primitive::Long ||
                         primitive == Primitive::ULong ||
                         primitive == Primitive::Float ||
                         primitive == Primitive::Double);

            const auto dt = static_cast<spark_datatype_t>(datatype);
            const auto op = static_cast<spark_operator_t>(Operator::Assignment);

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
        spark_node_t* copy_constructor(Datatype datatype, spark_node_t* that)
        {
            const auto dt = static_cast<spark_datatype_t>(datatype);
            const auto op = static_cast<spark_operator_t>(Operator::Assignment);

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
        spark_node_t* vector_constructor(Datatype datatype, spark_node_t** children, size_t count)
        {
            const auto dt = static_cast<spark_datatype_t>(datatype);

            auto listNode = spark_create_vector_node(dt, children, count, Spark::Internal::ThrowOnError());
            auto thisNode = copy_constructor(dt, listNode);
            return thisNode;
        }

    }
}