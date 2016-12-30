#pragma once

namespace Spark
{
    namespace NodeType
    {
        enum Type
        {
            Invalid = -1,
            Control,
            Operator,
            Function,
            Symbol,
            Constant,
            Property,
            Comment,
            Vector,
            ScopeBlock,

            Count
        };
    }
    typedef enum NodeType::Type nodetype_t;
}

typedef uint32_t spark_symbolid_t;

using Spark::nodetype_t;
using Spark::datatype_t;
using Spark::property_t;
using Spark::control_t;
using Spark::operator_t;

struct spark_node
{
    ~spark_node();
    std::vector<spark_node*> _children;
    nodetype_t _type = Spark::NodeType::Invalid;
    bool _attached = false;
    union
    {
        control_t _control;
        struct
        {
            datatype_t type;
            operator_t id;
        } _operator;
        struct
        {
            spark_symbolid_t id;
            datatype_t returnType;
            bool entrypoint;
        } _function;
        struct
        {
            datatype_t type;
            spark_symbolid_t id;
        } _symbol;
        struct
        {
            datatype_t type;
            uint8_t* buffer;
            uint32_t size;
        } _constant;
        struct
        {
            property_t id;
        } _property;
        struct
        {
            datatype_t type;
        } _vector;
        const char* _comment;
    };
};

typedef struct spark_node spark_node_t;