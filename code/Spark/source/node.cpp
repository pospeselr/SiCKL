#include "spark.hpp"

// spark internal
#include "node.hpp"
#include "error.hpp"
#include "text_utilities.hpp"

namespace spark
{
    namespace lib
    {
        thread_local spark_symbolid_t g_nextSymbol;
        thread_local std::vector<spark_node_t*> g_nodeStack;
        thread_local std::vector<spark_node_t*> g_allocatedNodes;

        spark_node::~spark_node()
        {
            if(this->_type == spark_nodetype::constant)
            {
                delete[] this->_constant.buffer;
            }
        }
    }
}

using namespace spark;
using namespace spark::lib;
using namespace spark::shared;

RUFF_EXPORT void spark_begin_program(spark_error_t** error)
{
    return TranslateExceptions(
        error,
        [&]
        {
            g_nextSymbol = 0;
        });
}

RUFF_EXPORT void spark_end_program(spark_error_t** error)
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

RUFF_EXPORT spark_node_t* spark_create_control_node(spark_control_t c, spark_error_t** error)
{
    return TranslateExceptions(
        error,
        [&]
        {
            SPARK_ASSERT(c < static_cast<spark_control_t>(Control::Count));

            spark_node_t* node = new spark_node_t();
            node->_type = spark_nodetype::control;
            node->_control = static_cast<Control>(c);

            g_allocatedNodes.push_back(node);
            return node;
        });
}

RUFF_EXPORT spark_node_t* spark_create_operator_node(spark_datatype_t dt, spark_operator_t id, spark_error_t** error)
{
    return TranslateExceptions(
        error,
        [&]
        {
            spark_node_t* node = new spark_node_t();
            node->_type = spark_nodetype::operation;
            node->_operator.type = static_cast<Datatype>(dt);
            node->_operator.id = static_cast<Operator>(id);

            g_allocatedNodes.push_back(node);
            return node;
        });
}

RUFF_EXPORT spark_node_t* spark_create_function_node(spark_datatype_t returnType, spark_error_t** error)
{
    return TranslateExceptions(
        error,
        [&]
        {
            spark_node_t* node = new spark_node_t();
            node->_type = spark_nodetype::function;
            node->_function.id = g_nextSymbol++;
            node->_function.returnType = returnType;
            node->_function.entrypoint = false;

            g_allocatedNodes.push_back(node);
            return node;
        });
}

RUFF_EXPORT spark_node_t* spark_create_symbol_node(spark_datatype_t dt, spark_error_t** error)
{
    return TranslateExceptions(
        error,
        [&]
        {
            spark_node_t* node = new spark_node_t();
            node->_type = spark_nodetype::symbol;
            node->_symbol.type = static_cast<Datatype>(dt);
            node->_symbol.id = g_nextSymbol++;

            g_allocatedNodes.push_back(node);
            return node;
        });
}

RUFF_EXPORT spark_node_t* spark_create_constant_node(spark_datatype_t dt, const void* raw, size_t sz, spark_error_t** error)
{
    return TranslateExceptions(
        error,
        [&]
        {
            SPARK_ASSERT(raw != nullptr);
            SPARK_ASSERT(sz > 0);

            spark_node_t* node = new spark_node_t();
            node->_type = spark_nodetype::constant;
            node->_constant.type = static_cast<Datatype>(dt);
            node->_constant.buffer = new uint8_t[sz];
            std::memcpy(node->_constant.buffer, raw, sz);
            node->_constant.size = sz;

            g_allocatedNodes.push_back(node);
            return node;
        });
}

RUFF_EXPORT spark_node_t* spark_create_property_node(spark_property_t prop, spark_error_t** error)
{
    return TranslateExceptions(
        error,
        [&]
        {
            spark_node_t* node = new spark_node_t();
            node->_type = spark_nodetype::property;
            node->_property.id = static_cast<Property>(prop);

            g_allocatedNodes.push_back(node);
            return node;
        });
}

RUFF_EXPORT spark_node_t* spark_create_comment_node(const char* comment, spark_error_t** error)
{
    return TranslateExceptions(
        error,
        [&]
        {
            spark_node_t* node = new spark_node_t();
            node->_type = spark_nodetype::comment;
            node->_comment = comment;

            g_allocatedNodes.push_back(node);
            return node;
        });
}

RUFF_EXPORT spark_node_t* spark_create_vector_node(spark_datatype_t type, spark_node_t** children, size_t count, spark_error_t** error)
{
    return TranslateExceptions(
        error,
        [&]
        {
            auto dt = static_cast<Datatype>(type);
            auto c = dt.GetComponents();
            SPARK_ASSERT(c != Components::None && c != Components::Scalar);
            spark_node_t* node = new spark_node_t();
            node->_type = spark_nodetype::vector;
            node->_vector.type = type;

            for(size_t k = 0; k < count; k++)
            {
                node->_children.push_back(children[k]);
            }

            g_allocatedNodes.push_back(node);
            return node;
        });
}

RUFF_EXPORT spark_node_t* spark_create_scope_block_node(spark_error_t** error)
{
    return TranslateExceptions(
        error,
        [&]
        {
            spark_node_t* node = new spark_node_t();
            node->_type = spark_nodetype::scope_block;

            g_allocatedNodes.push_back(node);
            return node;
        });
}

// tree modification
RUFF_EXPORT void spark_add_child_node(spark_node_t* root, spark_node_t* node, spark_error_t** error)
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
RUFF_EXPORT bool spark_node_get_attached(spark_node_t* node, spark_error_t** error)
{
    return TranslateExceptions(
        error,
        [&]
        {
            return node->_attached;
        });
}

RUFF_EXPORT void spark_node_make_entrypoint(spark_node_t* node, spark_error_t** error)
{
    return TranslateExceptions(
        error,
        [&]
        {
            node->_function.entrypoint = true;
        });
}

// source scope
RUFF_EXPORT void spark_push_scope_node(spark_node_t* node, spark_error_t** error)
{
    return TranslateExceptions(
        error,
        [&]
        {
            g_nodeStack.push_back(node);
        });
}

RUFF_EXPORT void spark_pop_scope_node(spark_error_t** error)
{
    return TranslateExceptions(
        error,
        [&]
        {
            g_nodeStack.pop_back();
        });
}

RUFF_EXPORT spark_node_t* spark_peek_scope_node(spark_error_t** error)
{
    return TranslateExceptions(
        error,
        [&]
        {
            return g_nodeStack.back();
        });
}

RUFF_EXPORT spark_node_t* spark_get_root_node(spark_error_t** error)
{
    return TranslateExceptions(
        error,
        [&]
        {
            return g_nodeStack.front();
        });
}
