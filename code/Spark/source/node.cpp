#include "spark.hpp"

// spark internal
#include "node.hpp"
#include "text_utilities.hpp"
#include "error.hpp"

spark_node::~spark_node()
{
    if(this->_type == Spark::NodeType::Constant)
    {
        delete[] this->_constant.buffer;
    }
}

namespace Spark
{
    namespace Internal
    {
        thread_local spark_symbolid_t g_nextSymbol;
        thread_local std::vector<spark_node_t*> g_nodeStack;
        thread_local std::vector<spark_node_t*> g_allocatedNodes;

        // node text functions
        int32_t nodeToText(spark_node_t* node, char* out_buffer, int32_t buffer_size, int32_t written, uint32_t bars, int32_t indentation);
    }
}

using namespace Spark;
using namespace Spark::Internal;

void spark_begin_program(spark_error_t** error)
{
    return TranslateExceptions(
        error,
        [&]
        {
            g_nextSymbol = 0;
        });
}

void spark_end_program(spark_error_t** error)
{
    return TranslateExceptions(
        error,
        [&]
        {
            // free all of our unattached nodes
            for(auto node : g_allocatedNodes)
            {
                delete node;
            }
            g_allocatedNodes.clear();
        });
}

// node creation

spark_node_t* spark_create_control_node(control_t c, spark_error_t** error)
{
    return TranslateExceptions(
        error,
        [&]
        {

            SPARK_ASSERT(c < Control::Count);

            spark_node_t* node = new spark_node_t();
            node->_type = NodeType::Control;
            node->_control = c;

            g_allocatedNodes.push_back(node);
            return node;
        });
}

spark_node_t* spark_create_operator_node(datatype_t dt, operator_t id, spark_error_t** error)
{
    return TranslateExceptions(
        error,
        [&]
        {
            spark_node_t* node = new spark_node_t();
            node->_type = NodeType::Operator;
            node->_operator.type = dt;
            node->_operator.id = id;

            g_allocatedNodes.push_back(node);
            return node;
        });
}

spark_node_t* spark_create_function_node(datatype_t returnType, spark_error_t** error)
{
    return TranslateExceptions(
        error,
        [&]
        {
            spark_node_t* node = new spark_node_t();
            node->_type = NodeType::Function;
            node->_function.id = g_nextSymbol++;
            node->_function.returnType = returnType;
            node->_function.entrypoint = false;

            g_allocatedNodes.push_back(node);
            return node;
        });
}

spark_node_t* spark_create_symbol_node(datatype_t dt, spark_error_t** error)
{
    return TranslateExceptions(
        error,
        [&]
        {
            spark_node_t* node = new spark_node_t();
            node->_type = NodeType::Symbol;
            node->_symbol.type = dt;
            node->_symbol.id = g_nextSymbol++;

            g_allocatedNodes.push_back(node);
            return node;
        });
}

spark_node_t* spark_create_constant_node(datatype_t dt, const void* raw, size_t sz, spark_error_t** error)
{
    return TranslateExceptions(
        error,
        [&]
        {
            SPARK_ASSERT(raw != nullptr);
            SPARK_ASSERT(sz > 0);

            spark_node_t* node = new spark_node_t();
            node->_type = NodeType::Constant;
            node->_constant.type = dt;
            node->_constant.buffer = new uint8_t[sz];
            std::memcpy(node->_constant.buffer, raw, sz);
            node->_constant.size = sz;

            g_allocatedNodes.push_back(node);
            return node;
        });
}

spark_node_t* spark_create_property_node(property_t prop, spark_error_t** error)
{
    return TranslateExceptions(
        error,
        [&]
        {
            spark_node_t* node = new spark_node_t();
            node->_type = NodeType::Property;
            node->_property.id = prop;

            g_allocatedNodes.push_back(node);
            return node;
        });
}

spark_node_t* spark_create_comment_node(const char* comment, spark_error_t** error)
{
    return TranslateExceptions(
        error,
        [&]
        {
            spark_node_t* node = new spark_node_t();
            node->_type = NodeType::Comment;
            node->_comment = comment;

            g_allocatedNodes.push_back(node);
            return node;
        });
}

spark_node_t* spark_create_vector_node(datatype_t type, spark_node_t** children, size_t count, spark_error_t** error)
{
    return TranslateExceptions(
        error,
        [&]
        {
            SPARK_ASSERT((type & DataType::ComponentMask) != 0);
            spark_node_t* node = new spark_node_t();
            node->_type = NodeType::Vector;
            node->_vector.type = type;

            g_allocatedNodes.push_back(node);

            for(size_t k = 0; k < count; k++)
            {
                node->_children.push_back(children[k]);
            }

            return node;
        });
}

spark_node_t* spark_create_scope_block_node(spark_error_t** error)
{
    return TranslateExceptions(
        error,
        [&]
        {
            spark_node_t* node = new spark_node_t();
            node->_type = NodeType::ScopeBlock;
            return node;
        });
}

// tree modification
void spark_add_child_node(spark_node_t* root, spark_node_t* node, spark_error_t** error)
{
    return TranslateExceptions(
        error,
        [&]
        {
            SPARK_ASSERT(root != nullptr);
            SPARK_ASSERT(node != nullptr);
            SPARK_ASSERT(root != node);

            node->_attached = true;
            root->_children.push_back(node);
        });
}

// node property query
bool spark_node_get_attached(spark_node_t* node, spark_error_t** error)
{
    return TranslateExceptions(
        error,
        [&]
        {
            return node->_attached;
        });
}

void spark_node_make_entrypoint(spark_node_t* node, spark_error_t** error)
{
    return TranslateExceptions(
        error,
        [&]
        {
            node->_function.entrypoint = true;
        });
}

// source scope
void spark_push_scope_node(spark_node_t* node, spark_error_t** error)
{
    return TranslateExceptions(
        error,
        [&]
        {
            g_nodeStack.push_back(node);
        });
}

void spark_pop_scope_node(spark_error_t** error)
{
    return TranslateExceptions(
        error,
        [&]
        {
            g_nodeStack.pop_back();
        });
}

spark_node_t* spark_peek_scope_node(spark_error_t** error)
{
    return TranslateExceptions(
        error,
        [&]
        {
            return g_nodeStack.back();
        });
}

spark_node_t* spark_get_root_node(spark_error_t** error)
{
    return TranslateExceptions(
        error,
        [&]
        {
            return g_nodeStack.front();
        });
}
