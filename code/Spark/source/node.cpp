#include "spark.hpp"

// spark internal
#include "node.hpp"
#include "text_utilities.hpp"

namespace Spark
{
    namespace Internal
    {
        thread_local symbolid_t g_nextSymbol;
        thread_local std::vector<Spark::Node*> g_nodeStack;
        thread_local std::vector<Spark::Node*> g_allocatedNodes;

        // node text functions
        int32_t nodeToText(Spark::Node* node, char* out_buffer, int32_t buffer_size, int32_t written, uint32_t bars, int32_t indentation);
    }
}

using namespace Spark;
using namespace Spark::Internal;

void spark_begin_program()
{
    g_nextSymbol = 0;
}

void spark_end_program()
{
    // free all of our unattached nodes
    for(auto node : g_allocatedNodes)
    {
        spark_free_node(node);
    }
    g_allocatedNodes.clear();
}

// symbol functions

Spark::symbolid_t spark_next_symbol()
{
    return g_nextSymbol++;
}

// node creation

Node* spark_create_control_node(control_t c)
{
    SPARK_ASSERT(c < Control::Count);

    Node* node = new Node();
    node->_type = NodeType::Control;
    node->_control = c;

    try
    {
        g_allocatedNodes.push_back(node);
    }
    catch(std::exception& ex)
    {
        spark_print_exception(ex);
    }

    return node;
}

Node* spark_create_operator_node(datatype_t dt, operator_t id)
{
    Node* node = new Node();
    node->_type = NodeType::Operator;
    node->_operator.type = dt;
    node->_operator.id = id;

    try
    {
        g_allocatedNodes.push_back(node);
    }
    catch(std::exception& ex)
    {
        spark_print_exception(ex);
    }

    return node;
}

Node* spark_create_function_node(symbolid_t id)
{
    Node* node = new Node();
    node->_type = NodeType::Function;
    node->_function.id = id;

    try
    {
        g_allocatedNodes.push_back(node);
    }
    catch(std::exception& ex)
    {
        spark_print_exception(ex);
    }

    return node;
}

Node* spark_create_symbol_node(datatype_t dt, symbolid_t id)
{
    Node* node = new Node();
    node->_type = NodeType::Symbol;
    node->_symbol.type = dt;
    node->_symbol.id = id;

    try
    {
        g_allocatedNodes.push_back(node);
    }
    catch(std::exception& ex)
    {
        spark_print_exception(ex);
    }

    return node;
}

Node* spark_create_constant_node(datatype_t dt, const void* raw, size_t sz)
{
    SPARK_ASSERT(raw != nullptr);
    SPARK_ASSERT(sz > 0);

    Node* node = new Node();
    node->_type = NodeType::Constant;
    node->_constant.type = dt;
    node->_constant.buffer = new uint8_t[sz];
    std::memcpy(node->_constant.buffer, raw, sz);
    node->_constant.size = sz;

    try
    {
        g_allocatedNodes.push_back(node);
    }
    catch(std::exception& ex)
    {
        spark_print_exception(ex);
    }

    return node;
}

Node* spark_create_property_node(property_t prop)
{
    Node* node = new Node();
    node->_type = NodeType::Property;
    node->_property.id = prop;

    try
    {
        g_allocatedNodes.push_back(node);
    }
    catch(std::exception& ex)
    {
        spark_print_exception(ex);
    }

    return node;
}

Node* spark_create_comment_node(const char* comment)
{
    Node* node = new Node();
    node->_type = NodeType::Comment;
    node->_comment = comment;

    try
    {
        g_allocatedNodes.push_back(node);
    }
    catch(std::exception& ex)
    {
        spark_print_exception(ex);
    }

    return node;
}

Node* spark_create_operator1_node(Spark::datatype_t dt, Spark::operator_t op, Spark::Node* arg1)
{
    Node* result_node = spark_create_operator_node(dt, op);
    spark_add_child_node(result_node, arg1);
    return result_node;
}

Node* spark_create_operator2_node(Spark::datatype_t dt, Spark::operator_t op, Spark::Node* arg1, Spark::Node* arg2)
{
    Node* result_node = spark_create_operator_node(dt, op);
    spark_add_child_node(result_node, arg1);
    spark_add_child_node(result_node, arg2);
    return result_node;
}

// node deletion
void spark_free_node(Node* node)
{
    if(node->_type == NodeType::Constant)
    {
        delete[] node->_constant.buffer;
    }
    delete node;
}
// tree modification
void spark_add_child_node(Node* root, Node* node)
{
    SPARK_ASSERT(root != nullptr);
    SPARK_ASSERT(node != nullptr);
    SPARK_ASSERT(root != node);

    root->_children.push_back(node);
}

// node property query

Spark::nodetype_t spark_node_get_type(Spark::Node* node)
{
    return node->_type;
}

Spark::operator_t spark_node_get_operator_id(Spark::Node* node)
{
    return node->_operator.id;
}

bool spark_node_get_attached(Spark::Node* node)
{
    return node->_attached;
}

Spark::symbolid_t spark_node_get_function_id(Spark::Node* node)
{
    return node->_function.id;
}

void spark_node_make_entrypoint(Spark::Node* node)
{
    node->_function.entrypoint = true;
}

// source scope
void spark_push_scope_node(Node* node)
{
    try
    {
        g_nodeStack.push_back(node);
    }
    catch(std::exception& ex)
    {
        spark_print_exception(ex);
    }
}

void spark_pop_scope_node()
{
    SPARK_ASSERT(g_nodeStack.size() > 0);
    g_nodeStack.pop_back();
}

Node* spark_peek_scope_node()
{
    SPARK_ASSERT(g_nodeStack.size() > 0);
    return g_nodeStack.back();
}

Node* spark_get_root_node()
{
    SPARK_ASSERT(g_nodeStack.size() > 0);
    return g_nodeStack.front();
}

int32_t spark_node_to_text(Spark::Node* node, char* out_buffer, int32_t buffer_size)
{
    return Spark::Internal::nodeToText(node, out_buffer, buffer_size, 0, 0, 0) + 1;
}
