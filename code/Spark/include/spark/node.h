#pragma once


namespace Spark
{
	struct Node;
}

// program scope
extern "C" void spark_begin_program(spark_error_t** error);
extern "C" void spark_end_program(spark_error_t** error);

// symbol functions
extern "C" Spark::symbolid_t spark_next_symbol(spark_error_t** error);

// node creation
extern "C" Spark::Node* spark_create_control_node(Spark::control_t c, spark_error_t** error);
extern "C" Spark::Node* spark_create_operator_node(Spark::datatype_t dt, Spark::operator_t id, spark_error_t** error);
extern "C" Spark::Node* spark_create_function_node(Spark::symbolid_t id, Spark::datatype_t returnType, spark_error_t** error);
extern "C" Spark::Node* spark_create_symbol_node(Spark::datatype_t dt, Spark::symbolid_t id, spark_error_t** error);
extern "C" Spark::Node* spark_create_constant_node(Spark::datatype_t dt, const void* raw, size_t sz, spark_error_t** error);
extern "C" Spark::Node* spark_create_property_node(Spark::property_t, spark_error_t** error);
extern "C" Spark::Node* spark_create_comment_node(const char* comment, spark_error_t** error);
extern "C" Spark::Node* spark_create_list_node(Spark::Node** children, size_t count, spark_error_t** error);

// node deletion
extern "C" void spark_free_node(Spark::Node* node, spark_error_t** error);
// tree modification
extern "C" void spark_add_child_node(Spark::Node* root, Spark::Node* node, spark_error_t** error);

// node property query
extern "C" Spark::nodetype_t spark_node_get_type(Spark::Node* node, spark_error_t** error);
extern "C" Spark::operator_t spark_node_get_operator_id(Spark::Node* node, spark_error_t** error);
extern "C" bool spark_node_get_attached(Spark::Node* node, spark_error_t** error);
extern "C" Spark::symbolid_t spark_node_get_function_id(Spark::Node* node, spark_error_t** error);
// node property set
extern "C" void spark_node_make_entrypoint(Spark::Node* node, spark_error_t** error);

// source scope
extern "C" void spark_push_scope_node(Spark::Node* node, spark_error_t** error);
extern "C" void spark_pop_scope_node(spark_error_t** error);
extern "C" Spark::Node* spark_peek_scope_node(spark_error_t** error);
extern "C" Spark::Node* spark_get_root_node(spark_error_t** error);

// compound node creation
inline Spark::Node* spark_create_operator1_node(Spark::datatype_t dt, Spark::operator_t op, Spark::Node* arg1)
{
    auto result_node = spark_create_operator_node(dt, op, Spark::Internal::ThrowOnError());
    spark_add_child_node(result_node, arg1, Spark::Internal::ThrowOnError());
    return result_node;
}
inline Spark::Node* spark_create_operator2_node(Spark::datatype_t dt, Spark::operator_t op, Spark::Node* arg1, Spark::Node* arg2)
{
    auto result_node = spark_create_operator_node(dt, op, Spark::Internal::ThrowOnError());
    spark_add_child_node(result_node, arg1, Spark::Internal::ThrowOnError());
    spark_add_child_node(result_node, arg2, Spark::Internal::ThrowOnError());
    return result_node;
}
