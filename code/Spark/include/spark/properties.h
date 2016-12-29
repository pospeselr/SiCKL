#pragma once

namespace Spark
{
    namespace Internal
    {
        void property_assignment_operator(property_t id, datatype_t dt, spark_node_t* thisNode, spark_node_t* thatNode)
        {
            // create property node
            auto property = spark_create_property_node(id, Spark::Internal::ThrowOnError());
            auto lvalue = spark_create_operator2_node(dt, Operator::Property, thisNode, property);

            // create assignment node
            const auto op = Operator::Assignment;
            auto assignmentNode = spark_create_operator2_node(dt, op, lvalue, thatNode);

            // add to tree
            auto currentScope = spark_peek_scope_node(Spark::Internal::ThrowOnError());
            spark_add_child_node(currentScope, assignmentNode, Spark::Internal::ThrowOnError());
        }
    }

    template<typename TYPE, property_t ID>
    struct property_r
    {
        __attribute__((always_inline))
        operator const rvalue<TYPE>() const
        {
            auto property = spark_create_property_node(ID, Spark::Internal::ThrowOnError());
            const auto dt = TYPE::type;
            const auto op = Operator::Property;

            return rvalue<TYPE>(spark_create_operator2_node(dt, op, this->_node, property));
        };

        __attribute__((always_inline))
        const rvalue<TYPE> operator()() const
        {
            auto property = spark_create_property_node(ID, Spark::Internal::ThrowOnError());
            const auto dt = TYPE::type;
            const auto op = Operator::Property;

            return rvalue<TYPE>(spark_create_operator2_node(dt, op, this->_node, property));
        }
        spark_node_t* _node;
    };

    template<typename TYPE, property_t ID>
    struct property_rw
    {
        __attribute__((always_inline))
        operator lvalue<TYPE>() const
        {
            auto property = spark_create_property_node(ID, Spark::Internal::ThrowOnError());
            const auto dt = TYPE::type;
            const auto op = Operator::Property;

            return lvalue<TYPE>(spark_create_operator2_node(dt, op, this->_node, property));
        };

        __attribute__((always_inline))
        lvalue<TYPE> operator()() const
        {
            auto property = spark_create_property_node(ID, Spark::Internal::ThrowOnError());
            const auto dt = TYPE::type;
            const auto op = Operator::Property;

            return lvalue<TYPE>(spark_create_operator2_node(dt, op, this->_node, property));
        }

        __attribute__((always_inline))
        property_rw& operator=(const TYPE& right)
        {
            Internal::property_assignment_operator(ID, TYPE::type, this->_node, right._node);
            return *this;
        }
        spark_node_t* _node;
    };
}