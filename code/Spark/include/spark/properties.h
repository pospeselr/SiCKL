#pragma once

namespace Spark
{
    template<typename TYPE, property_t ID>
    struct property_r
    {
        operator const rvalue<TYPE>() const
        {
            Node* property = spark_create_property_node(ID);
            const auto dt = TYPE::type;
            const auto op = Operator::Property;

            return rvalue<TYPE>(spark_create_operator2_node(dt, op, this->_node, property));
        };

        const rvalue<TYPE> operator()() const
        {
            Node* property = spark_create_property_node(ID);
            const auto dt = TYPE::type;
            const auto op = Operator::Property;

            return rvalue<TYPE>(spark_create_operator2_node(dt, op, this->_node, property));
        }
        Node* _node;
    };

    template<typename TYPE, property_t ID>
    struct property_rw
    {
        operator TYPE() const
        {
            Node* property = spark_create_property_node(ID);
            const auto dt = TYPE::type;
            const auto op = Operator::Property;

            return TYPE(spark_create_operator2_node(dt, op, this->_node, property));
        };

        TYPE operator()() const
        {
            Node* property = spark_create_property_node(ID);
            const auto dt = TYPE::type;
            const auto op = Operator::Property;

            return TYPE(spark_create_operator2_node(dt, op, this->_node, property));
        }

        property_rw& operator=(const TYPE& right)
        {
            // create property node
            Node* property = spark_create_property_node(ID);
            Node* lvalue = spark_create_operator2_node(TYPE::type, Operator::Property, this->_node, property);

            // create assignment node
            const auto dt = TYPE::type;
            const auto op = Operator::Assignment;
            Node* assignmentNode = spark_create_operator2_node(dt, op, lvalue, right._node);

            // add to tree
            Node* currentScope = spark_peek_scope_node();
            spark_add_child_node(currentScope, assignmentNode);

            return *this;
        }
        Node* _node;
    };
}