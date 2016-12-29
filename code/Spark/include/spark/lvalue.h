#pragma once

namespace Spark
{
    template<typename TYPE>
    struct lvalue : TYPE
    {
        lvalue(spark_node_t* node) : TYPE(node) {}
        using TYPE::operator=;
    };
}