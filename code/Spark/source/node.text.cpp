#include "spark.hpp"

// spark internal
#include "node.hpp"
#include "text_utilities.hpp"

namespace Spark
{
    namespace Internal
    {
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
                "0x%x -> function%s\n",
                node->_function.id,
                node->_function.entrypoint ? "(entrypoint)" : "");
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

        int32_t commentNodeToText(Spark::Node* node, char* buffer, int32_t buffer_size, int32_t written)
        {
            written = doSnprintf(buffer, buffer_size, written, "Comment: '%s'\n", node->_comment);
            return written;
        }

        // if out_bufer is null
        int32_t nodeToText(Spark::Node* node, char* out_buffer, int32_t buffer_size, int32_t written, uint32_t bars, int32_t indentation)
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
                case NodeType::Comment:
                    written = commentNodeToText(node, out_buffer, buffer_size, written);
                    break;
                default:
                    written = doSnprintf(out_buffer, buffer_size, written, "%s\n", spark_nodetype_to_str(node->_type));
                    break;
            }

            // depth first traversal
            for(uint32_t k = 0; k < node->_childCount; k++)
            {
                Node* child = node->_children[k];
                const uint32_t childBars =
                    (k < node->_childCount - 1) ?
                    bars | (1 << indentation) :
                    bars;
                const int32_t childIndentation = indentation + 1;

                //uint32_t childBars = 0;
                written = nodeToText(child, out_buffer, buffer_size, written, childBars, childIndentation);
            }

            // return characters written
            return written;
        }
    }
}