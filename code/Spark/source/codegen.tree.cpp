#include "spark.hpp"

// spark internal
#include "node.hpp"
#include "error.hpp"
#include "text_utilities.hpp"

const char* spark_nodetype_to_str(spark_nodetype_t val);

using namespace spark;
using namespace spark::lib;
using namespace spark::shared;

namespace spark
{
    namespace lib
    {
        int32_t controlNodeToText(spark_node_t* node, char* buffer, int32_t buffer_size, int32_t written)
        {
            return doSnprintf(buffer, buffer_size, written,
                "%s\n",
                spark_control_to_str(static_cast<spark_control_t>(node->_control)));
        }

        int32_t operatorNodeToText(spark_node_t* node, char* buffer, int32_t buffer_size, int32_t written)
        {
            const auto dt = static_cast<spark_datatype_t>(node->_operator.type);
            const auto op = static_cast<spark_operator_t>(node->_operator.id);

            char datatypeBuffer[32];
            return doSnprintf(buffer, buffer_size, written,
                "%s : %s\n",
                spark_operator_to_str(op),
                spark_datatype_to_str(dt, datatypeBuffer, sizeof(datatypeBuffer)));
        }

        int32_t functionNodeToText(spark_node_t* node, char* buffer, int32_t buffer_size, int32_t written)
        {
            const auto dt = static_cast<spark_datatype_t>(node->_function.returnType);

            char datatypeBuffer[32];
            return doSnprintf(buffer, buffer_size, written,
                "0x%x : function -> %s %s\n",
                (uint32_t)node->_function.id,
                spark_datatype_to_str(dt, datatypeBuffer, sizeof(datatypeBuffer)),
                node->_function.entrypoint ? "(entrypoint)" : "");
        }

        int32_t symbolNodeToText(spark_node_t* node, char* buffer, int32_t buffer_size, int32_t written)
        {
            const auto dt = static_cast<spark_datatype_t>(node->_symbol.type);

            char datatypeBuffer[32];
            return doSnprintf(buffer, buffer_size, written,
                "0x%x : %s\n",
                (uint32_t)node->_symbol.id,
                spark_datatype_to_str(dt, datatypeBuffer, sizeof(datatypeBuffer)));
        }

        int32_t constantNodeToText(spark_node_t* node, char* buffer, int32_t buffer_size, int32_t written)
        {
            const auto dt = node->_constant.type;
            const auto primitive = dt.GetPrimitive();
            // primitive cannot be void
            SPARK_ASSERT(primitive != Primitive::Void);
            const auto components = dt.GetComponents();;
            const auto pointer = dt.GetPointer();
            SPARK_ASSERT(pointer == false);


            int32_t componentCount = 1;
            switch(components)
            {
                case Components::None:
                    componentCount = 0;
                    break;
                case Components::Scalar:
                    componentCount = 1;
                    break;
                case Components::Vector2:
                    componentCount = 2;
                    break;
                case Components::Vector4:
                    componentCount = 4;
                    break;
                case Components::Vector8:
                    componentCount = 8;
                    break;
                case Components::Vector16:
                    componentCount = 16;
                    break;
                default:
                    SPARK_ASSERT(false);
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
                    case Primitive::Char:
                        signed_integer = *((int8_t*)raw + k);
                        break;
                    case Primitive::UChar:
                        unsigned_integer = *((uint8_t*)raw + k);
                        break;
                    case Primitive::Short:
                        signed_integer = *((int16_t*)raw + k);
                        break;
                    case Primitive::UShort:
                        unsigned_integer = *((uint16_t*)raw + k);
                        break;
                    case Primitive::Int:
                        signed_integer = *((int32_t*)raw + k);
                        break;
                    case Primitive::UInt:
                        unsigned_integer = *((uint32_t*)raw + k);
                        break;
                    case Primitive::Long:
                        signed_integer = *((int64_t*)raw + k);
                        break;
                    case Primitive::ULong:
                        unsigned_integer = *((uint64_t*)raw + k);
                        break;
                    case Primitive::Float:
                        floating_point = *((float*)raw + k);
                        break;
                    case Primitive::Double:
                        floating_point = *((double*)raw + k);
                        break;
                    default:
                        SPARK_ASSERT(false);
                }

                switch(primitive)
                {
                    case Primitive::Char:
                    case Primitive::Short:
                    case Primitive::Int:
                    case Primitive::Long:
                        written = doSnprintf(buffer, buffer_size, written, "%lli", signed_integer);
                        break;
                    case Primitive::UChar:
                    case Primitive::UShort:
                    case Primitive::UInt:
                    case Primitive::ULong:
                        written = doSnprintf(buffer, buffer_size, written, "%llu", unsigned_integer);
                        break;
                    case Primitive::Float:
                    case Primitive::Double:
                        written = doSnprintf(buffer, buffer_size, written, "%f", floating_point);
                        break;
                    default:
                        SPARK_ASSERT(false);
                }
            }

            char datatypeBuffer[32];
            written = doSnprintf(buffer, buffer_size, written,
                " : %s\n",
                spark_datatype_to_str(static_cast<spark_datatype_t>(dt), datatypeBuffer, sizeof(datatypeBuffer)));

            return written;
        }

        int32_t propertyNodeToText(spark_node_t* node, char* buffer, int32_t buffer_size, int32_t written)
        {
            const auto prop = static_cast<spark_property_t>(node->_property.id);

            char propertyBuffer[8];
            written = doSnprintf(buffer, buffer_size, written,
                "Property::%s\n",
                spark_property_to_str(prop, propertyBuffer, countof(propertyBuffer)));
            return written;
        }

        int32_t commentNodeToText(spark_node_t* node, char* buffer, int32_t buffer_size, int32_t written)
        {
            written = doSnprintf(buffer, buffer_size, written, "NodeType::Comment : '%s'\n", node->_comment);
            return written;
        }

        int32_t vectorNodeToText(spark_node_t* node, char* buffer, int32_t buffer_size, int32_t written)
        {
            const auto dt = static_cast<spark_datatype_t>(node->_vector.type);

            char datatypeBuffer[32];
            written = doSnprintf(buffer, buffer_size, written, "NodeType::Vector : '%s'\n", spark_datatype_to_str(dt, datatypeBuffer, sizeof(datatypeBuffer)));
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
                case spark_nodetype::control:
                    written = controlNodeToText(node, out_buffer, buffer_size, written);
                    break;
                case spark_nodetype::operation:
                    written = operatorNodeToText(node, out_buffer, buffer_size, written);
                    break;
                case spark_nodetype::function:
                    written = functionNodeToText(node, out_buffer, buffer_size, written);
                    break;
                case spark_nodetype::symbol:
                    written = symbolNodeToText(node, out_buffer, buffer_size, written);
                    break;
                case spark_nodetype::constant:
                    written = constantNodeToText(node, out_buffer, buffer_size, written);
                    break;
                case spark_nodetype::property:
                    written = propertyNodeToText(node, out_buffer, buffer_size, written);
                    break;
                case spark_nodetype::comment:
                    written = commentNodeToText(node, out_buffer, buffer_size, written);
                    break;
                case spark_nodetype::vector:
                    written = vectorNodeToText(node, out_buffer, buffer_size, written);
                    break;
                default:
                    written = doSnprintf(out_buffer, buffer_size, written, "%s\n", spark_nodetype_to_str(node->_type));
                    break;
            }

            // only 1 function node (no copy with just symbol id), so only print children if we're
            // defining the function
            bool printChildren =
                ((nodeType == spark_nodetype::function) &&
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

DLL_PUBLIC int32_t spark_node_to_text(spark_node_t* node, char* out_buffer, int32_t buffer_size, spark_error_t** error)
{
    return TranslateExceptions(
        error,
        [&]
        {
            return nodeToText(node, out_buffer, buffer_size, 0, 0, 0) + 1;
        });
}