#pragma once

namespace Spark
{
    struct Node
    {
        ~Node();
        std::vector<Node*> _children;
        nodetype_t _type = NodeType::Invalid;
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
                symbolid_t id;
                datatype_t returnType;
                bool entrypoint;
            } _function;
            struct
            {
                datatype_t type;
                symbolid_t id;
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
}