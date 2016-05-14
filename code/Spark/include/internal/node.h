#pragma once


namespace Spark
{
	struct Node
    {
        Node** _children;;
        uint32_t _childCount;
        uint32_t _bufferSize;
        nodetype_t _type;
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
                uint8_t* buffer;
                uint32_t size;
            } _constant;
        };
    };
}


// program scope
extern "C" void spark_begin_program();
extern "C" void spark_end_program();

// symbol functions
extern "C" Spark::symbolid_t spark_next_symbol();

// node creation
extern "C" Spark::Node* spark_create_control_node(Spark::control_t c);
extern "C" Spark::Node* spark_create_function_node(Spark::datatype_t dt, Spark::symbolid_t id);
extern "C" Spark::Node* spark_create_symbol_node(Spark::datatype_t dt, Spark::symbolid_t id);
extern "C" Spark::Node* spark_create_constant_node(Spark::datatype_t dt, const void* raw, size_t sz);
// node deletion
extern "C" void spark_free_node(Spark::Node* node);
// tree modification
extern "C" void spark_add_child_node(Spark::Node* root, Spark::Node* node);
// source scope
extern "C" void spark_push_scope_node(Spark::Node* node);
extern "C" void spark_pop_scope_node();
extern "C" Spark::Node* spark_peek_scope_node();
extern "C" Spark::Node* spark_get_root_node();
// debug printing
extern "C" int32_t spark_node_to_text(Spark::Node* node, char* buffer, int32_t buffer_size);

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