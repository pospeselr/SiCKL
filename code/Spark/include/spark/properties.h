#pragma once

namespace Spark
{
    namespace Internal
    {
        void property_assignment_operator(Property property, Datatype datatype, spark_node_t* thisNode, spark_node_t* thatNode)
        {
            const auto prop = static_cast<spark_property_t>(property);
            const auto dt = static_cast<spark_datatype_t>(datatype);

            // create property node
            auto propertyNode = spark_create_property_node(prop, Spark::Internal::ThrowOnError());
            auto lvalue = spark_create_operator2_node(dt, static_cast<spark_operator_t>(Operator::Property), thisNode, propertyNode);

            // create assignment node
            const auto op = Operator::Assignment;
            auto assignmentNode = spark_create_operator2_node(dt, static_cast<spark_operator_t>(op), lvalue, thatNode);

            // add to tree
            auto currentScope = spark_peek_scope_node(Spark::Internal::ThrowOnError());
            spark_add_child_node(currentScope, assignmentNode, Spark::Internal::ThrowOnError());
        }
    }

    template<typename TYPE, Spark::Internal::Property ID>
    struct property_r
    {
        __attribute__((always_inline))
        operator const rvalue<TYPE>() const
        {
            const auto prop = static_cast<spark_property_t>(ID);
            auto propertyNode = spark_create_property_node(prop, Spark::Internal::ThrowOnError());

            const auto dt = static_cast<spark_datatype_t>(TYPE::type);
            const auto op = static_cast<spark_operator_t>(Spark::Internal::Operator::Property);

            return rvalue<TYPE>(spark_create_operator2_node(dt, op, this->_node, propertyNode));
        };

        __attribute__((always_inline))
        const rvalue<TYPE> operator()() const
        {
            const auto prop = static_cast<spark_property_t>(ID);
            auto propertyNode = spark_create_property_node(prop, Spark::Internal::ThrowOnError());

            const auto dt = static_cast<spark_datatype_t>(TYPE::type);
            const auto op = static_cast<spark_operator_t>(Spark::Internal::Operator::Property);

            return rvalue<TYPE>(spark_create_operator2_node(dt, op, this->_node, propertyNode));
        }
        spark_node_t* _node;
    };

    template<typename TYPE, Spark::Internal::Property ID>
    struct property_rw : public property_r<TYPE, ID>
    {
        __attribute__((always_inline))
        operator lvalue<TYPE>()
        {
            const auto prop = static_cast<spark_property_t>(ID);
            auto propertyNode = spark_create_property_node(prop, Spark::Internal::ThrowOnError());

            const auto dt = static_cast<spark_datatype_t>(TYPE::type);
            const auto op = static_cast<spark_operator_t>(Spark::Internal::Operator::Property);

            return lvalue<TYPE>(spark_create_operator2_node(dt, op, this->_node, propertyNode));
        };

        __attribute__((always_inline))
        lvalue<TYPE> operator()()
        {
            const auto prop = static_cast<spark_property_t>(ID);
            auto propertyNode = spark_create_property_node(prop, Spark::Internal::ThrowOnError());

            const auto dt = static_cast<spark_datatype_t>(TYPE::type);
            const auto op = static_cast<spark_operator_t>(Spark::Internal::Operator::Property);

            return lvalue<TYPE>(spark_create_operator2_node(dt, op, this->_node, propertyNode));
        }

        __attribute__((always_inline))
        property_rw& operator=(const rvalue<TYPE>& right)
        {
            Internal::property_assignment_operator(ID, TYPE::type, this->_node, right._node);
            return *this;
        }
    };
}