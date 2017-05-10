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
            auto thisNode = spark_create_symbol_node(dt, SPARK_THROW_ON_ERROR());
            return thisNode;
        }

        inline
        SPARK_NEVER_INLINE
        spark_node_t* value_constructor(spark::shared::Datatype datatype, const void* raw, size_t sz)
        {
            SPARK_ASSERT(datatype.GetPrimitive() == spark::shared::Primitive::Char ||
                         datatype.GetPrimitive() == spark::shared::Primitive::UChar ||
                         datatype.GetPrimitive() == spark::shared::Primitive::Short ||
                         datatype.GetPrimitive() == spark::shared::Primitive::UShort ||
                         datatype.GetPrimitive() == spark::shared::Primitive::Int ||
                         datatype.GetPrimitive() == spark::shared::Primitive::UInt ||
                         datatype.GetPrimitive() == spark::shared::Primitive::Long ||
                         datatype.GetPrimitive() == spark::shared::Primitive::ULong ||
                         datatype.GetPrimitive() == spark::shared::Primitive::Float ||
                         datatype.GetPrimitive() == spark::shared::Primitive::Double);

            const auto dt = static_cast<spark_datatype_t>(datatype);
            const auto op = static_cast<spark_operator_t>(spark::shared::Operator::Assignment);

            auto thisNode = spark_create_symbol_node(dt, SPARK_THROW_ON_ERROR());

            // init to val
            auto valNode = spark_create_constant_node(dt, raw, sz, SPARK_THROW_ON_ERROR());

            // create assignment node
            auto assignmentNode = spark_create_operator2_node(dt, op, thisNode, valNode);
            // add to tree
            auto currentScope = spark_peek_scope_node(SPARK_THROW_ON_ERROR());
            spark_add_child_node(currentScope, assignmentNode, SPARK_THROW_ON_ERROR());

            return thisNode;
        }

        inline
        SPARK_NEVER_INLINE
        spark_node_t* constant_constructor(spark::shared::Datatype datatype, void* raw, size_t sz)
        {
            SPARK_ASSERT(datatype.GetPrimitive() == spark::shared::Primitive::Char ||
                         datatype.GetPrimitive() == spark::shared::Primitive::UChar ||
                         datatype.GetPrimitive() == spark::shared::Primitive::Short ||
                         datatype.GetPrimitive() == spark::shared::Primitive::UShort ||
                         datatype.GetPrimitive() == spark::shared::Primitive::Int ||
                         datatype.GetPrimitive() == spark::shared::Primitive::UInt ||
                         datatype.GetPrimitive() == spark::shared::Primitive::Long ||
                         datatype.GetPrimitive() == spark::shared::Primitive::ULong ||
                         datatype.GetPrimitive() == spark::shared::Primitive::Float ||
                         datatype.GetPrimitive() == spark::shared::Primitive::Double);

            const auto dt = static_cast<spark_datatype_t>(datatype);
            return spark_create_constant_node(dt, raw, sz, SPARK_THROW_ON_ERROR());
        }

        template<typename T>
        inline
        SPARK_FORCE_INLINE
        spark_node_t* constant_constructor(spark::shared::Datatype datatype, T val)
        {
            return constant_constructor(datatype, &val, sizeof(val));
        }

        inline
        SPARK_NEVER_INLINE
        spark_node_t* copy_constructor(spark::shared::Datatype datatype, spark_node_t* that)
        {
            const auto dt = static_cast<spark_datatype_t>(datatype);
            const auto op = static_cast<spark_operator_t>(spark::shared::Operator::Assignment);

            auto thisNode = spark_create_symbol_node(dt, SPARK_THROW_ON_ERROR());
            auto thatNode = that;

            // create assignment node
            auto assignmentNode = spark_create_operator2_node(dt, op, thisNode, thatNode);
            // add to tree
            auto currentScope = spark_peek_scope_node(SPARK_THROW_ON_ERROR());
            spark_add_child_node(currentScope, assignmentNode, SPARK_THROW_ON_ERROR());

            return thisNode;
        }

        inline
        SPARK_NEVER_INLINE
        spark_node_t* vector_constructor(spark::shared::Datatype datatype, std::initializer_list<spark_node_t*> children)
        {
            const auto dt = static_cast<spark_datatype_t>(datatype);

            auto listNode = spark_create_vector_node(dt, const_cast<spark_node_t**>(children.begin()), children.size(), SPARK_THROW_ON_ERROR());
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
            auto currentScope = spark_peek_scope_node(SPARK_THROW_ON_ERROR());
            spark_add_child_node(currentScope, assignmentNode, SPARK_THROW_ON_ERROR());
        }
    }
}