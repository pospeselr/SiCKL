#pragma once

namespace spark
{
    namespace client
    {
        // special type used to specify extern construtor in kernel types
        enum class extern_construct_t { value };
        constexpr auto extern_construct = extern_construct_t::value;

        // special type used to specify null constructor in kernel types
        enum class null_construct_t { value };
        constexpr auto null_construct = null_construct_t::value;

        inline
        SPARK_NEVER_INLINE
        spark_node_t* extern_constructor(spark::shared::Datatype datatype)
        {
            const auto dt = static_cast<spark_datatype_t>(datatype);
            auto thisNode = spark_create_symbol_node(dt, THROW_ON_ERROR());
            return thisNode;
        }

        inline
        SPARK_NEVER_INLINE
        spark_node_t* value_constructor(spark::shared::Datatype datatype, const void* raw, size_t sz)
        {
            auto primitive = datatype.GetPrimitive();

            SPARK_ASSERT(primitive == spark::shared::Primitive::Char ||
                         primitive == spark::shared::Primitive::UChar ||
                         primitive == spark::shared::Primitive::Short ||
                         primitive == spark::shared::Primitive::UShort ||
                         primitive == spark::shared::Primitive::Int ||
                         primitive == spark::shared::Primitive::UInt ||
                         primitive == spark::shared::Primitive::Long ||
                         primitive == spark::shared::Primitive::ULong ||
                         primitive == spark::shared::Primitive::Float ||
                         primitive == spark::shared::Primitive::Double);

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
        SPARK_NEVER_INLINE
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
        SPARK_NEVER_INLINE
        spark_node_t* vector_constructor(spark::shared::Datatype datatype, spark_node_t** children, size_t count)
        {
            const auto dt = static_cast<spark_datatype_t>(datatype);

            auto listNode = spark_create_vector_node(dt, children, count, THROW_ON_ERROR());
            auto thisNode = copy_constructor(dt, listNode);
            return thisNode;
        }

        inline
        SPARK_NEVER_INLINE
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