#pragma once

namespace Spark
{
    namespace Internal
    {
        template<typename TYPE>
        struct lvalue : TYPE
        {
            lvalue(spark_node_t* node) : TYPE(node) {}
            using TYPE::operator=;
        };
    }
}