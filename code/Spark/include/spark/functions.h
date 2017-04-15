#pragma once

namespace spark
{
    const Int2 Index()
    {
        const auto dt = static_cast<spark_datatype_t>(Int::type);
        const auto op = static_cast<spark_operator_t>(spark::shared::Operator::Index);
        return client::rvalue<Int2>(spark_create_operator_node(dt, op, THROW_ON_ERROR()));
    }

    const Float2 NormalizedIndex()
    {
        const auto dt = static_cast<spark_datatype_t>(Int::type);
        const auto op = static_cast<spark_operator_t>(spark::shared::Operator::NormalizedIndex);
        return client::rvalue<Float2>(spark_create_operator_node(dt, op, THROW_ON_ERROR()));
    }
}