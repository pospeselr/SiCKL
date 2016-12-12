#pragma once


namespace Spark
{
	struct Node;
}

// program scope
extern "C" void spark_begin_program();
extern "C" void spark_end_program();

// symbol functions
extern "C" Spark::symbolid_t spark_next_symbol();

// node creation
extern "C" Spark::Node* spark_create_control_node(Spark::control_t c);
extern "C" Spark::Node* spark_create_operator_node(Spark::datatype_t dt, Spark::operator_t id);
extern "C" Spark::Node* spark_create_function_node(Spark::symbolid_t id);
extern "C" Spark::Node* spark_create_symbol_node(Spark::datatype_t dt, Spark::symbolid_t id);
extern "C" Spark::Node* spark_create_constant_node(Spark::datatype_t dt, const void* raw, size_t sz);
extern "C" Spark::Node* spark_create_property_node(Spark::property_t);
extern "C" Spark::Node* spark_create_comment_node(const char* comment);

// node deletion
extern "C" void spark_free_node(Spark::Node* node);
// tree modification
extern "C" void spark_add_child_node(Spark::Node* root, Spark::Node* node);

// node property query
extern "C" Spark::nodetype_t spark_node_get_type(Spark::Node* node);
extern "C" Spark::operator_t spark_node_get_operator_id(Spark::Node* node);
extern "C" bool spark_node_get_attached(Spark::Node* node);
extern "C" Spark::symbolid_t spark_node_get_function_id(Spark::Node* node);
// node property set
extern "C" void spark_node_make_entrypoint(Spark::Node* node);

// source scope
extern "C" void spark_push_scope_node(Spark::Node* node);
extern "C" void spark_pop_scope_node();
extern "C" Spark::Node* spark_peek_scope_node();
extern "C" Spark::Node* spark_get_root_node();
// debug printing
extern "C" int32_t spark_node_to_text(Spark::Node* node, char* buffer, int32_t buffer_size);
extern "C" int32_t spark_node_to_json(Spark::Node* node, char* buffer, int32_t buffer_size);

// compound node creation
inline Spark::Node* spark_create_operator1_node(Spark::datatype_t dt, Spark::operator_t op, Spark::Node* arg1)
{
    auto result_node = spark_create_operator_node(dt, op);
    spark_add_child_node(result_node, arg1);
    return result_node;
}
inline Spark::Node* spark_create_operator2_node(Spark::datatype_t dt, Spark::operator_t op, Spark::Node* arg1, Spark::Node* arg2)
{
    auto result_node = spark_create_operator_node(dt, op);
    spark_add_child_node(result_node, arg1);
    spark_add_child_node(result_node, arg2);
    return result_node;
}
