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
                Spark::Internal::Control _control;
                struct
                {
                    Spark::Internal::Datatype type;
                    Spark::Internal::Operator id;
                } _operator;
                struct
                {
                    spark_symbolid_t id;
                    Spark::Internal::Datatype returnType;
                    bool entrypoint;
                } _function;
                struct
                {
                    Spark::Internal::Datatype type;
                    spark_symbolid_t id;
                } _symbol;
                struct
                {
                    Spark::Internal::Datatype type;
                    uint8_t* buffer;
                    uint32_t size;
                } _constant;
                struct
                {
                    Spark::Internal::Property id;
                } _property;
                struct
                {
                    Spark::Internal::Datatype type;
                } _vector;
                const char* _comment;
            };
        };
    }
}

typedef spark::lib::spark_nodetype spark_nodetype_t;
typedef struct spark::lib::spark_node spark_node_t;