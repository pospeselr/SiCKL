#pragma once

namespace spark
{
    namespace client
    {
        struct statement
        {
            operator bool() const
            {
                return true;
            }
        };

        struct if_statement : public statement
        {
            if_statement(const rvalue<Int>& val)
            {
                auto root = spark_create_control_node(static_cast<spark_control_t>(spark::shared::Control::If), THROW_ON_ERROR());
                spark_add_child_node(spark_peek_scope_node(THROW_ON_ERROR()), root, THROW_ON_ERROR());
                spark_push_scope_node(root, THROW_ON_ERROR());

                spark_add_child_node(root, val._node, THROW_ON_ERROR());

                auto body = spark_create_scope_block_node(THROW_ON_ERROR());
                spark_add_child_node(root, body, THROW_ON_ERROR());
                spark_push_scope_node(body, THROW_ON_ERROR());
            }

            ~if_statement()
            {
                spark_pop_scope_node(THROW_ON_ERROR());
                spark_pop_scope_node(THROW_ON_ERROR());
            }
        };

        struct elseif_statement : public statement
        {
            elseif_statement(const rvalue<Int>& val)
            {
                auto root = spark_create_control_node(static_cast<spark_control_t>(spark::shared::Control::ElseIf), THROW_ON_ERROR());
                spark_add_child_node(spark_peek_scope_node(THROW_ON_ERROR()), root, THROW_ON_ERROR());
                spark_push_scope_node(root, THROW_ON_ERROR());

                spark_add_child_node(root, val._node, THROW_ON_ERROR());

                auto body = spark_create_scope_block_node(THROW_ON_ERROR());
                spark_add_child_node(root, body, THROW_ON_ERROR());
                spark_push_scope_node(body, THROW_ON_ERROR());
            }

            ~elseif_statement()
            {
                spark_pop_scope_node(THROW_ON_ERROR());
                spark_pop_scope_node(THROW_ON_ERROR());
            }
        };

        struct else_statement : public statement
        {
            else_statement()
            {
                auto root = spark_create_control_node(static_cast<spark_control_t>(spark::shared::Control::Else), THROW_ON_ERROR());
                spark_add_child_node(spark_peek_scope_node(THROW_ON_ERROR()), root, THROW_ON_ERROR());
                spark_push_scope_node(root, THROW_ON_ERROR());

                auto body = spark_create_scope_block_node(THROW_ON_ERROR());
                spark_add_child_node(root, body, THROW_ON_ERROR());
                spark_push_scope_node(body, THROW_ON_ERROR());
            }

            ~else_statement()
            {
                spark_pop_scope_node(THROW_ON_ERROR());
                spark_pop_scope_node(THROW_ON_ERROR());
            }
        };

        struct while_statement : public statement
        {
            while_statement(const rvalue<Int>& val)
            {
                auto root = spark_create_control_node(static_cast<spark_control_t>(spark::shared::Control::While), THROW_ON_ERROR());
                spark_add_child_node(spark_peek_scope_node(THROW_ON_ERROR()), root, THROW_ON_ERROR());
                spark_push_scope_node(root, THROW_ON_ERROR());

                spark_add_child_node(root, val._node, THROW_ON_ERROR());

                auto body = spark_create_scope_block_node(THROW_ON_ERROR());
                spark_add_child_node(root, body, THROW_ON_ERROR());
                spark_push_scope_node(body, THROW_ON_ERROR());
            }

            ~while_statement()
            {
                spark_pop_scope_node(THROW_ON_ERROR());
                spark_pop_scope_node(THROW_ON_ERROR());
            }
        };
    }

    #define If(X)     if (auto __if_##__LINE__ = spark::client::if_statement(X))
    #define ElseIf(X) if (auto __elseif_##__LINE__ = spark::client::elseif_statement(X))
    #define Else      if (auto __else_##__LINE__ = spark::client::else_statement())
    #define While(X)  if (auto __while_##__LINE__ = spark::client::while_statement(X))
    #define For(X)    for(X)

    void Break()
    {
        const auto dt = static_cast<spark_datatype_t>(spark::shared::Datatype(spark::shared::Primitive::Void, spark::shared::Components::None, false));
        const auto op = static_cast<spark_operator_t>(spark::shared::Operator::Break);

        auto breakNode = spark_create_operator_node(dt, op, THROW_ON_ERROR());
        spark_add_child_node(spark_peek_scope_node(THROW_ON_ERROR()), breakNode, THROW_ON_ERROR());
    }

}