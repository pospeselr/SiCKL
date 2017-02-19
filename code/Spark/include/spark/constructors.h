#pragma once

namespace spark
{
    namespace client
    {
        inline
        __attribute__ ((noinline))
        spark_node_t* extern_constructor(spark::shared::Datatype datatype)
        {
            const auto dt = static_cast<spark_datatype_t>(datatype);
            auto thisNode = spark_create_symbol_node(dt, THROW_ON_ERROR());
            return thisNode;
        }

        inline
        __attribute__ ((noinline))
        spark_node_t* value_constructor(spark::shared::Datatype datatype, const void* raw, size_t sz)
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
            const auto op = static_cast<spark_operator_t>(spark::shared::Operator::Assignment);

            auto thisNode = spark_create_symbol_node(dt, THROW_ON_ERROR());

            // init to val
            auto valNode = spark_create_constant_node(dt, raw, sz, THROW_ON_ERROR());

            // create assignment node
            auto assignmentNode = spark_create_operator2_node(dt, op, thisNode, valNode);
            // add to tree
            auto currentScope = spark_peek_scope_node(THROW_ON_ERROR());
            spark_add_child_node(currentScope, assignmentNode, THROW_ON_ERROR());

            return thisNode;
        }

        inline
        __attribute__ ((noinline))
        spark_node_t* copy_constructor(spark::shared::Datatype datatype, spark_node_t* that)
        {
            const auto dt = static_cast<spark_datatype_t>(datatype);
            const auto op = static_cast<spark_operator_t>(spark::shared::Operator::Assignment);

            auto thisNode = spark_create_symbol_node(dt, THROW_ON_ERROR());
            auto thatNode = that;

            // create assignment node
            auto assignmentNode = spark_create_operator2_node(dt, op, thisNode, thatNode);
            // add to tree
            auto currentScope = spark_peek_scope_node(THROW_ON_ERROR());
            spark_add_child_node(currentScope, assignmentNode, THROW_ON_ERROR());

            return thisNode;
        }

        inline
        __attribute__ ((noinline))
        spark_node_t* vector_constructor(spark::shared::Datatype datatype, spark_node_t** children, size_t count)
        {
            const auto dt = static_cast<spark_datatype_t>(datatype);

            auto listNode = spark_create_vector_node(dt, children, count, THROW_ON_ERROR());
            auto thisNode = copy_constructor(dt, listNode);
            return thisNode;
        }

        inline
        __attribute__ ((noinline))
        void assignment_operator(spark_node_t* thisNode, spark::shared::Datatype datatype, spark_node_t* thatNode)
        {
            const auto dt = static_cast<spark_datatype_t>(datatype);
            const auto op = static_cast<spark_operator_t>(spark::shared::Operator::Assignment);

            // create assignment node
            auto assignmentNode = spark_create_operator2_node(dt, op, thisNode, thatNode);
            // add to tree
            auto currentScope = spark_peek_scope_node(THROW_ON_ERROR());
            spark_add_child_node(currentScope, assignmentNode, THROW_ON_ERROR());
        }
    }
}