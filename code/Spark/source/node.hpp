#pragma once

namespace spark
{
    namespace lib
    {
        enum class spark_nodetype : uint32_t
        {
            invalid = ~uint32_t(0u),
            control = 0,
            operation,
            function,
            symbol,
            constant,
            property,
            comment,
            vector,
            scope_block,

            count
        };

        typedef uint32_t spark_symbolid_t;

        struct spark_node
        {
            spark_node() {};
            ~spark_node();
            std::vector<spark_node*> _children;
            spark_nodetype _type;
            bool _attached = false;
            union
            {
                spark::shared::Control _control;
                struct
                {
                    spark::shared::Datatype type;
                    spark::shared::Operator id;
                } _operator;
                struct
                {
                    spark_symbolid_t id;
                    spark::shared::Datatype returnType;
                    bool entrypoint;
                } _function;
                struct
                {
                    spark::shared::Datatype type;
                    spark_symbolid_t id;
                } _symbol;
                struct
                {
                    spark::shared::Datatype type;
                    uint8_t* buffer;
                    uint32_t size;
                } _constant;
                struct
                {
                    spark::shared::Property id;
                } _property;
                struct
                {
                    spark::shared::Datatype type;
                } _vector;
                const char* _comment;
            };
        };
    }
}

typedef spark::lib::spark_nodetype spark_nodetype_t;
typedef struct spark::lib::spark_node spark_node_t;