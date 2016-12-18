#pragma once

namespace Spark
{
    template<typename TYPE>
    struct lvalue : TYPE
    {
        lvalue(Node* node) : TYPE(node) {}
        using TYPE::operator=;
    };
}