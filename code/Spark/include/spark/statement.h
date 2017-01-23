#pragma once

namespace Spark
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
            auto root = spark_create_control_node(static_cast<spark_control_t>(Internal::Control::If), Spark::Internal::ThrowOnError());
            spark_add_child_node(spark_peek_scope_node(Spark::Internal::ThrowOnError()), root, Spark::Internal::ThrowOnError());
            spark_push_scope_node(root, Spark::Internal::ThrowOnError());

            spark_add_child_node(root, val._node, Spark::Internal::ThrowOnError());

            auto body = spark_create_scope_block_node(Spark::Internal::ThrowOnError());
            spark_add_child_node(root, body, Spark::Internal::ThrowOnError());
            spark_push_scope_node(body, Spark::Internal::ThrowOnError());
        }

        ~if_statement()
        {
            spark_pop_scope_node(Spark::Internal::ThrowOnError());
            spark_pop_scope_node(Spark::Internal::ThrowOnError());
        }
    };

    struct elseif_statement : public statement
    {
        elseif_statement(const rvalue<Int>& val)
        {
            auto root = spark_create_control_node(static_cast<spark_control_t>(Internal::Control::ElseIf), Spark::Internal::ThrowOnError());
            spark_add_child_node(spark_peek_scope_node(Spark::Internal::ThrowOnError()), root, Spark::Internal::ThrowOnError());
            spark_push_scope_node(root, Spark::Internal::ThrowOnError());

            spark_add_child_node(root, val._node, Spark::Internal::ThrowOnError());

            auto body = spark_create_scope_block_node(Spark::Internal::ThrowOnError());
            spark_add_child_node(root, body, Spark::Internal::ThrowOnError());
            spark_push_scope_node(body, Spark::Internal::ThrowOnError());
        }

        ~elseif_statement()
        {
            spark_pop_scope_node(Spark::Internal::ThrowOnError());
            spark_pop_scope_node(Spark::Internal::ThrowOnError());
        }
    };

    struct else_statement : public statement
    {
        else_statement()
        {
            auto root = spark_create_control_node(static_cast<spark_control_t>(Internal::Control::Else), Spark::Internal::ThrowOnError());
            spark_add_child_node(spark_peek_scope_node(Spark::Internal::ThrowOnError()), root, Spark::Internal::ThrowOnError());
            spark_push_scope_node(root, Spark::Internal::ThrowOnError());

            auto body = spark_create_scope_block_node(Spark::Internal::ThrowOnError());
            spark_add_child_node(root, body, Spark::Internal::ThrowOnError());
            spark_push_scope_node(body, Spark::Internal::ThrowOnError());
        }

        ~else_statement()
        {
            spark_pop_scope_node(Spark::Internal::ThrowOnError());
            spark_pop_scope_node(Spark::Internal::ThrowOnError());
        }
    };

    struct while_statement : public statement
    {
        while_statement(const rvalue<Int>& val)
        {
            auto root = spark_create_control_node(static_cast<spark_control_t>(Internal::Control::While), Spark::Internal::ThrowOnError());
            spark_add_child_node(spark_peek_scope_node(Spark::Internal::ThrowOnError()), root, Spark::Internal::ThrowOnError());
            spark_push_scope_node(root, Spark::Internal::ThrowOnError());

            spark_add_child_node(root, val._node, Spark::Internal::ThrowOnError());

            auto body = spark_create_scope_block_node(Spark::Internal::ThrowOnError());
            spark_add_child_node(root, body, Spark::Internal::ThrowOnError());
            spark_push_scope_node(body, Spark::Internal::ThrowOnError());
        }

        ~while_statement()
        {
            spark_pop_scope_node(Spark::Internal::ThrowOnError());
            spark_pop_scope_node(Spark::Internal::ThrowOnError());
        }
    };

    #define If(X)     if (auto __if_##__LINE__ = if_statement(X))
    #define ElseIf(X) if (auto __elseif_##__LINE__ = elseif_statement(X))
    #define Else      if (auto __else_##__LINE__ = else_statement())
    #define While(X)  if (auto __while_##__LINE__ = while_statement(X))
    #define For(X)    for(X)

    void Break()
    {
        const auto dt = static_cast<spark_datatype_t>(Datatype(Primitive::Void, Components::None, false));
        const auto op = static_cast<spark_operator_t>(Operator::Break);

        auto breakNode = spark_create_operator_node(dt, op, Spark::Internal::ThrowOnError());
        spark_add_child_node(spark_peek_scope_node(Spark::Internal::ThrowOnError()), breakNode, Spark::Internal::ThrowOnError());
    }

}