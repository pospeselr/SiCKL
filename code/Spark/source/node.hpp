#pragma once

namespace Spark
{
    struct Node
    {
        Node** _children;
        uint32_t _childCount;
        uint32_t _bufferSize;
        nodetype_t _type;
        bool _attached;
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
            const char* _comment;
        };
    };
}