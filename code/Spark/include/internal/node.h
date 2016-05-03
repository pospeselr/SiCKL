#pragma once

namespace Spark
{
	struct Node
    {
        Node* _children;
        Node* _next;
        nodetype_t _nodeType;
        union
        {
            control_t _control;
            struct
            {
                datatype_t type;
                symbolid_t id;
            } _function;
            struct
            {
                datatype_t type;
                symbolid_t id;
            } _symbol;
            struct
            {
                datatype_t type;
                void* buffer;
                uint32_t     size;
            } _constant;
        };
    };
}


// program scope
extern "C" void spark_begin_program();
extern "C" void spark_end_program();

// node creation
extern "C" Spark::Node* spark_create_control_node(Spark::control_t c);
extern "C" Spark::Node* spark_create_function_node(Spark::datatype_t dt, Spark::symbolid_t id);
extern "C" Spark::Node* spark_create_symbol_node(Spark::datatype_t dt, Spark::symbolid_t id);
extern "C" Spark::Node* spark_create_constant_node(Spark::datatype_t dt, void* raw, size_t sz);
// node deletion
extern "C" void spark_free_node(Spark::Node* node);
// tree modification
extern "C" void spark_add_child_node(Spark::Node* root, Spark::Node* node);
// source scope
extern "C" void spark_push_scope_node(Spark::Node* node);
extern "C" void spark_pop_scope_node();
extern "C" Spark::Node* spark_peek_scope_node();

#if 0

Function
    ParamList
        A
        B
        C
    Block
        Operator=
            Symbol: D
            Operator+
                A
                B
        If
            D
            Block
                Function
                    A
                    B
        Else
            Block

        While
            Literal : true
            Block
                Derp
                Derp
                Derp


#endif