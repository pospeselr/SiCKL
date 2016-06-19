#include "precomp.h"

#include "internal/common.h"
#include "internal/enums.h"
#include "internal/node.h"
#include "internal/error.h"

namespace Spark
{
    namespace Internal
    {
        thread_local symbolid_t g_nextSymbol;
        thread_local std::vector<Spark::Node*> g_nodeStack;
        thread_local std::vector<Spark::Node*> g_allocatedNodes;

        uintptr_t isNull(char* buffer)
        {
            return buffer ? 1 : 0;
        }

        char* nullOrOffset(char* buffer, uint32_t offset)
        {
            return buffer + isNull(buffer) * offset;
        }

        template<typename... Args>
        int32_t doSnprintf(char* buffer, int32_t buffer_size, int32_t written, const Args&... args)
        {
            SPARK_ASSERT(written >= 0);
            auto len = snprintf(nullOrOffset(buffer, written),  buffer_size == 0 ? 0 : buffer_size - written, args...);
            SPARK_ASSERT(len >= 0);
            SPARK_ASSERT(buffer_size == 0 || (written + len < buffer_size));

            return written + len;
        }

        int32_t controlNodeToText(Spark::Node* node, char* buffer, int32_t buffer_size, int32_t written)
        {
            return doSnprintf(buffer, buffer_size, written,
                "%s\n",
                spark_control_to_str(node->_control));
        }

        int32_t operatorNodeToText(Spark::Node* node, char* buffer, int32_t buffer_size, int32_t written)
        {
            char datatypeBuffer[32];
            return doSnprintf(buffer, buffer_size, written,
                "%s -> %s\n",
                spark_operator_to_str((operator_t)node->_operator.id),
                spark_datatype_to_str(node->_operator.type, datatypeBuffer, sizeof(datatypeBuffer)));
        }

        int32_t functionNodeToText(Spark::Node* node, char* buffer, int32_t buffer_size, int32_t written)
        {
            return doSnprintf(buffer, buffer_size, written,
                "0x%x -> function\n",
                node->_symbol.id);
        }

        int32_t symbolNodeToText(Spark::Node* node, char* buffer, int32_t buffer_size, int32_t written)
        {
            char datatypeBuffer[32];
            return doSnprintf(buffer, buffer_size, written,
                "0x%x -> %s\n",
                node->_symbol.id,
                spark_datatype_to_str(node->_symbol.type, datatypeBuffer, sizeof(datatypeBuffer)));
        }

        int32_t constantNodeToText(Spark::Node* node, char* buffer, int32_t buffer_size, int32_t written)
        {
            const auto dt = node->_constant.type;
            const auto primitive = dt & DataType::PrimitiveMask;
            // primitive cannot be void
            SPARK_ASSERT(primitive != DataType::Void);
            const auto component = dt & DataType::ComponentMask;
            const auto container = dt & DataType::ContainerMask;
            // container cannot be init'd with a constant
            SPARK_ASSERT(container == 0);

            int32_t componentCount = 1;
            switch(component)
            {
                case DataType::Vector2:
                    componentCount = 2;
                    break;
                case DataType::Vector3:
                    componentCount = 3;
                    break;
                case DataType::Vector4:
                    componentCount = 4;
                    break;
                case DataType::Vector8:
                    componentCount = 8;
                    break;
                case DataType::Vector16:
                    componentCount = 16;
                    break;
            }

            auto raw = node->_constant.buffer;
            SPARK_ASSERT(raw != nullptr);

            for(int32_t k = 0; k < componentCount; k++)
            {
                if(k > 0)
                {
                    written = doSnprintf(buffer, buffer_size, written, "%s", ", ");
                }

                union
                {
                    int64_t signed_integer;
                    uint64_t unsigned_integer;
                    double floating_point;
                };

                switch(primitive)
                {
                    case DataType::Char:
                        signed_integer = *((int8_t*)raw + k);
                        break;
                    case DataType::UChar:
                        unsigned_integer = *((uint8_t*)raw + k);
                        break;
                    case DataType::Short:
                        signed_integer = *((int16_t*)raw + k);
                        break;
                    case DataType::UShort:
                        unsigned_integer = *((uint16_t*)raw + k);
                        break;
                    case DataType::Int:
                        signed_integer = *((int32_t*)raw + k);
                        break;
                    case DataType::UInt:
                        unsigned_integer = *((uint32_t*)raw + k);
                        break;
                    case DataType::Long:
                        signed_integer = *((int64_t*)raw + k);
                        break;
                    case DataType::ULong:
                        unsigned_integer = *((uint64_t*)raw + k);
                        break;
                    case DataType::Float:
                        floating_point = *((float*)raw + k);
                        break;
                    case DataType::Double:
                        floating_point = *((double*)raw + k);
                        break;
                }

                switch(primitive)
                {
                    case DataType::Char:
                    case DataType::Short:
                    case DataType::Int:
                    case DataType::Long:
                        written = doSnprintf(buffer, buffer_size, written, "%lli", signed_integer);
                        break;
                    case DataType::UChar:
                    case DataType::UShort:
                    case DataType::UInt:
                    case DataType::ULong:
                        written = doSnprintf(buffer, buffer_size, written, "%llu", unsigned_integer);
                        break;
                    case DataType::Float:
                    case DataType::Double:
                        written = doSnprintf(buffer, buffer_size, written, "%f", floating_point);
                        break;
                }
            }

            char datatypeBuffer[32];
            written = doSnprintf(buffer, buffer_size, written,
                " -> %s\n",
                spark_datatype_to_str(node->_symbol.type, datatypeBuffer, sizeof(datatypeBuffer)));

            return written;
        }

        int32_t propertyNodeToText(Spark::Node* node, char* buffer, int32_t buffer_size, int32_t written)
        {
            char propertyBuffer[8];
            written = doSnprintf(buffer, buffer_size, written,
                "Property::%s\n",
                spark_property_to_str(node->_property.id, propertyBuffer, countof(propertyBuffer)));
            return written;
        }

        // if out_bufer is null
        int32_t nodeToText(Spark::Node* node, char* out_buffer, int32_t buffer_size, int32_t written, int32_t indentation)
        {
            SPARK_ASSERT(node != nullptr);
            SPARK_ASSERT((out_buffer == nullptr && buffer_size == 0) || (out_buffer != nullptr && buffer_size > 0));

            // write indentation
            for(int32_t k = 0; k < indentation; k++)
            {
                written = doSnprintf(out_buffer, buffer_size, written, "%s", " ");
            }
            // write node info

            switch(node->_type)
            {
                case NodeType::Control:
                    written = controlNodeToText(node, out_buffer, buffer_size, written);
                    break;
                case NodeType::Operator:
                    written = operatorNodeToText(node, out_buffer, buffer_size, written);
                    break;
                case NodeType::Function:
                    written = functionNodeToText(node, out_buffer, buffer_size, written);
                    break;
                case NodeType::Symbol:
                    written = symbolNodeToText(node, out_buffer, buffer_size, written);
                    break;
                case NodeType::Constant:
                    written = constantNodeToText(node, out_buffer, buffer_size, written);
                    break;
                case NodeType::Property:
                    written = propertyNodeToText(node, out_buffer, buffer_size, written);
                    break;
                default:
                    written = doSnprintf(out_buffer, buffer_size, written, "%s\n", spark_nodetype_to_str(node->_type));
                    break;
            }

            // depth first traversal
            for(uint32_t k = 0; k < node->_childCount; k++)
            {
                Node* child = node->_children[k];
                written = nodeToText(child, out_buffer, buffer_size, written, indentation + 1);
            }

            // return characters written
            return written;
        }
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
    node->_children = nullptr;
    node->_childCount = 0;
    node->_bufferSize = 0;
    node->_type = NodeType::Control;
    node->_attached = false;
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
    node->_children = nullptr;
    node->_childCount = 0;
    node->_bufferSize = 0;
    node->_type = NodeType::Operator;
    node->_attached = false;
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
    node->_children = nullptr;
    node->_childCount = 0;
    node->_bufferSize = 0;
    node->_type = NodeType::Function;
    node->_attached = false;
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
    node->_children = nullptr;
    node->_childCount = 0;
    node->_bufferSize = 0;
    node->_type = NodeType::Symbol;
    node->_attached = false;
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
    node->_children = nullptr;
    node->_childCount = 0;
    node->_bufferSize = 0;
    node->_type = NodeType::Constant;
    node->_attached = false;
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
    node->_children = nullptr;
    node->_childCount = 0;
    node->_bufferSize = 0;
    node->_attached = false;
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
    delete[] node->_children;
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

    // init buffer if it is empty
    if(root->_children == nullptr)
    {
        const uint32_t bufferSize = 8;
        root->_children = new Node*[bufferSize];
        for(uint32_t k = 0; k < bufferSize; k++)
        {
            root->_children[k] = nullptr;
        }
        root->_bufferSize = bufferSize;
        root->_childCount = 0;
    }

    // add child node
    root->_children[root->_childCount++] = node;
    node->_attached = true;

    // resize buffer if it is full
    if(root->_childCount == root->_bufferSize)
    {
        // copy over nodes to new buffer
        const uint32_t bufferSize = root->_bufferSize * 2;
        const uint32_t childCount = root->_childCount;
        Node** children = new Node*[bufferSize];
        for(uint32_t k = 0; k < childCount; k++)
        {
            children[k] = root->_children[k];
        }

        for(uint32_t k = childCount; k < bufferSize; k++)
        {
            children[k] = nullptr;
        }

        // delete old buffer and copy over data
        delete[] root->_children;
        root->_children = children;
        root->_childCount = childCount;
        root->_bufferSize = bufferSize;
    }

    SPARK_ASSERT(root->_childCount < root->_bufferSize);
}
// source scope
void spark_push_scope_node(Node* node)
{
    try
    {
        if(g_nodeStack.size() > 0)
        {
            spark_add_child_node(g_nodeStack.back(), node);
        }
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
    return Spark::Internal::nodeToText(node, out_buffer, buffer_size, 0, 0) + 1;
}
