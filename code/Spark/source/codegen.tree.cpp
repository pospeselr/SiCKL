#include "spark.hpp"

// spark internal
#include "node.hpp"
#include "text_utilities.hpp"
#include "error.hpp"

const char* spark_nodetype_to_str(Spark::nodetype_t val);

namespace Spark
{
    namespace Internal
    {
        int32_t controlNodeToText(spark_node_t* node, char* buffer, int32_t buffer_size, int32_t written)
        {
            return doSnprintf(buffer, buffer_size, written,
                "%s\n",
                spark_control_to_str(node->_control));
        }

        int32_t operatorNodeToText(spark_node_t* node, char* buffer, int32_t buffer_size, int32_t written)
        {
            char datatypeBuffer[32];
            return doSnprintf(buffer, buffer_size, written,
                "%s : %s\n",
                spark_operator_to_str((operator_t)node->_operator.id),
                spark_datatype_to_str(node->_operator.type, datatypeBuffer, sizeof(datatypeBuffer)));
        }

        int32_t functionNodeToText(spark_node_t* node, char* buffer, int32_t buffer_size, int32_t written)
        {
            char datatypeBuffer[32];
            return doSnprintf(buffer, buffer_size, written,
                "0x%x : function -> %s %s\n",
                (uint32_t)node->_function.id,
                spark_datatype_to_str(node->_function.returnType, datatypeBuffer, sizeof(datatypeBuffer)),
                node->_function.entrypoint ? "(entrypoint)" : "");
        }

        int32_t symbolNodeToText(spark_node_t* node, char* buffer, int32_t buffer_size, int32_t written)
        {
            char datatypeBuffer[32];
            return doSnprintf(buffer, buffer_size, written,
                "0x%x : %s\n",
                (uint32_t)node->_symbol.id,
                spark_datatype_to_str(node->_symbol.type, datatypeBuffer, sizeof(datatypeBuffer)));
        }

        int32_t constantNodeToText(spark_node_t* node, char* buffer, int32_t buffer_size, int32_t written)
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
                    written = doSnprintf(buffer, buffer_size, written, ", ");
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
                " : %s\n",
                spark_datatype_to_str(node->_symbol.type, datatypeBuffer, sizeof(datatypeBuffer)));

            return written;
        }

        int32_t propertyNodeToText(spark_node_t* node, char* buffer, int32_t buffer_size, int32_t written)
        {
            char propertyBuffer[8];
            written = doSnprintf(buffer, buffer_size, written,
                "Property::%s\n",
                spark_property_to_str(node->_property.id, propertyBuffer, countof(propertyBuffer)));
            return written;
        }

        int32_t commentNodeToText(spark_node_t* node, char* buffer, int32_t buffer_size, int32_t written)
        {
            written = doSnprintf(buffer, buffer_size, written, "NodeType::Comment : '%s'\n", node->_comment);
            return written;
        }

        int32_t vectorNodeToText(spark_node_t* node, char* buffer, int32_t buffer_size, int32_t written)
        {
            char datatypeBuffer[32];
            written = doSnprintf(buffer, buffer_size, written, "NodeType::Vector : '%s'\n", spark_datatype_to_str(node->_vector.type, datatypeBuffer, sizeof(datatypeBuffer)));
            return written;
        }

        // if out_bufer is null
        int32_t nodeToText(spark_node_t* node, char* out_buffer, int32_t buffer_size, int32_t written, uint32_t bars, int32_t indentation)
        {
            SPARK_ASSERT(node != nullptr);
            SPARK_ASSERT((out_buffer == nullptr && buffer_size == 0) || (out_buffer != nullptr && buffer_size > 0));

            // write graph lines
            for(int32_t k = 0; k < indentation; k++)
            {
                const bool printBar = (bars & (1 << k));
                const bool printLeaf = (k == indentation - 1);

                const char* str = "  ";
                if(printLeaf)
                {
                    if(!printBar)
                    {
                        str = "└─";
                    }
                    else
                    {
                        str = "├─";
                    }
                }
                else if(printBar)
                {
                    str = "│ ";
                }
                written = doSnprintf(out_buffer, buffer_size, written, "%s", str);
            }

            // write node info
            const auto nodeType = node->_type;
            switch(nodeType)
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
                case NodeType::Comment:
                    written = commentNodeToText(node, out_buffer, buffer_size, written);
                    break;
                case NodeType::Vector:
                    written = vectorNodeToText(node, out_buffer, buffer_size, written);
                    break;
                default:
                    written = doSnprintf(out_buffer, buffer_size, written, "%s\n", spark_nodetype_to_str(node->_type));
                    break;
            }

            // only 1 function node (no copy with just symbol id), so only print children if we're
            // defining the function
            bool printChildren =
                ((nodeType == NodeType::Function) &&
                 (indentation > 1)) ?
                false :
                true;

            if(printChildren)
            {
                // depth first traversal
                const size_t childCount = node->_children.size();
                for(size_t k = 0; k < childCount; k++)
                {
                    auto child = node->_children[k];
                    const uint32_t childBars =
                        (k < childCount - 1) ?
                        bars | (1 << indentation) :
                        bars;
                    const int32_t childIndentation = indentation + 1;

                    written = nodeToText(child, out_buffer, buffer_size, written, childBars, childIndentation);
                }
            }

            // return characters written
            return written;
        }
    }
}

int32_t spark_node_to_text(spark_node_t* node, char* out_buffer, int32_t buffer_size, spark_error_t** error)
{
    return Spark::Internal::TranslateExceptions(
        error,
        [&]
        {
            return Spark::Internal::nodeToText(node, out_buffer, buffer_size, 0, 0, 0) + 1;
        });
}