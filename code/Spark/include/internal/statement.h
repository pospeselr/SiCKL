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
        if_statement(const Int& val)
        {
            Node* root = spark_create_control_node(Control::If);
            spark_add_child_node(spark_peek_scope_node(), root);

            Node* parameterList = spark_create_control_node(Control::ParameterList);

            spark_add_child_node(root, parameterList);
            spark_add_child_node(parameterList, val._node);

            Node* body = spark_create_control_node(Control::ScopeBlock);
            spark_add_child_node(root, body);

            spark_push_scope_node(body);
        }

        ~if_statement()
        {
            spark_pop_scope_node();
        }
    };

    struct elseif_statement : public statement
    {
        elseif_statement(const Int& val)
        {
            Node* root = spark_create_control_node(Control::ElseIf);
            spark_add_child_node(spark_peek_scope_node(), root);

            Node* parameterList = spark_create_control_node(Control::ParameterList);

            spark_add_child_node(root, parameterList);
            spark_add_child_node(parameterList, val._node);

            Node* body = spark_create_control_node(Control::ScopeBlock);
            spark_add_child_node(root, body);

            spark_push_scope_node(body);
        }

        ~elseif_statement()
        {
            spark_pop_scope_node();
        }
    };

    struct else_statement : public statement
    {
        else_statement()
        {
            Node* root = spark_create_control_node(Control::Else);
            spark_add_child_node(spark_peek_scope_node(), root);

            Node* body = spark_create_control_node(Control::ScopeBlock);
            spark_add_child_node(root, body);

            spark_push_scope_node(body);
        }

        ~else_statement()
        {
            spark_pop_scope_node();
        }
    };

    struct while_statement : public statement
    {
        while_statement(const Int& val)
        {
            Node* root = spark_create_control_node(Control::While);
            spark_add_child_node(spark_peek_scope_node(), root);

            Node* parameterList = spark_create_control_node(Control::ParameterList);

            spark_add_child_node(root, parameterList);
            spark_add_child_node(parameterList, val._node);

            Node* body = spark_create_control_node(Control::ScopeBlock);
            spark_add_child_node(root, body);

            spark_push_scope_node(body);
        }

        ~while_statement()
        {
            spark_pop_scope_node();
        }
    };

    #define If(X)     if (auto __if_##__LINE__ = if_statement(X))
    #define ElseIf(X) if (auto __elseif_##__LINE__ = elseif_statement(X))
    #define Else      if (auto __else_##__LINE__ = else_statement())
    #define While(X)  if (auto __while_##__LINE__ = while_statement(X))

}