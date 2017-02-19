#pragma once

typedef struct spark_node spark_node_t;

// program scope
extern "C" void spark_begin_program(spark_error_t** error);
extern "C" void spark_end_program(spark_error_t** error);

// node creation
extern "C" spark_node_t* spark_create_control_node(spark_control_t ctrl, spark_error_t** error);
extern "C" spark_node_t* spark_create_operator_node(spark_datatype_t dt, spark_operator_t id, spark_error_t** error);
extern "C" spark_node_t* spark_create_function_node(spark_datatype_t returnType, spark_error_t** error);
extern "C" spark_node_t* spark_create_symbol_node(spark_datatype_t dt, spark_error_t** error);
extern "C" spark_node_t* spark_create_constant_node(spark_datatype_t dt, const void* raw, size_t sz, spark_error_t** error);
extern "C" spark_node_t* spark_create_property_node(spark_property_t, spark_error_t** error);
extern "C" spark_node_t* spark_create_comment_node(const char* comment, spark_error_t** error);
extern "C" spark_node_t* spark_create_vector_node(spark_datatype_t dt, spark_node_t** children, size_t count, spark_error_t** error);
extern "C" spark_node_t* spark_create_scope_block_node(spark_error_t** error);

// node deletion
extern "C" void spark_free_node(spark_node_t* node, spark_error_t** error);
// tree modification
extern "C" void spark_add_child_node(spark_node_t* root, spark_node_t* node, spark_error_t** error);

// node property query
extern "C" bool spark_node_get_attached(spark_node_t* node, spark_error_t** error);
// node property set
extern "C" void spark_node_make_entrypoint(spark_node_t* node, spark_error_t** error);

// source scope
extern "C" void spark_push_scope_node(spark_node_t* node, spark_error_t** error);
extern "C" void spark_pop_scope_node(spark_error_t** error);
extern "C" spark_node_t* spark_peek_scope_node(spark_error_t** error);
extern "C" spark_node_t* spark_get_root_node(spark_error_t** error);

// compound node creation
inline spark_node_t* spark_create_operator1_node(spark_datatype_t dt, spark_operator_t op, spark_node_t* arg1)
{
    auto result_node = spark_create_operator_node(dt, op, THROW_ON_ERROR());
    spark_add_child_node(result_node, arg1, THROW_ON_ERROR());
    return result_node;
}
inline spark_node_t* spark_create_operator2_node(spark_datatype_t dt, spark_operator_t op, spark_node_t* arg1, spark_node_t* arg2)
{
    auto result_node = spark_create_operator_node(dt, op, THROW_ON_ERROR());
    spark_add_child_node(result_node, arg1, THROW_ON_ERROR());
    spark_add_child_node(result_node, arg2, THROW_ON_ERROR());
    return result_node;
}
