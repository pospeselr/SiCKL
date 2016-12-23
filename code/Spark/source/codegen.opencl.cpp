#include "spark.hpp"

// spark internal
#include "node.hpp"
#include "text_utilities.hpp"
#include "error.hpp"

using std::string;

namespace Spark
{
    namespace Internal
    {
        int32_t generateSymbolName(symbolid_t id, datatype_t dt, char* out_buffer, int32_t buffer_size, int32_t written)
        {
            const auto primitive_component = dt & DataType::PrimitiveMask;
            const auto vector_component = dt & DataType::ComponentMask;
            const auto pointer_component = dt & DataType::PointerMask;

            const char* primitive = nullptr;
            switch(primitive_component)
            {
                case DataType::Char:    primitive = "s8_"; break;
                case DataType::UChar:   primitive = "u8_"; break;
                case DataType::Short:   primitive = "s16_"; break;
                case DataType::UShort:  primitive = "u16_"; break;
                case DataType::Int:     primitive = "s32_"; break;
                case DataType::UInt:    primitive = "u32_"; break;
                case DataType::Long:    primitive = "s64_"; break;
                case DataType::ULong:   primitive = "u64_"; break;
                case DataType::Float:   primitive = "flt_"; break;
                case DataType::Double:  primitive = "dbl_"; break;
            }
            SPARK_ASSERT(primitive != nullptr);

            const char* vector = nullptr;
            switch(vector_component)
            {
                case DataType::Vector2: vector = "vec2_"; break;
                default:                vector = ""; break;
            }

            const char* pointer = nullptr;
            switch(pointer_component)
            {
                case DataType::Pointer: pointer = "p_"; break;
                default:                pointer = ""; break;
            }

            return doSnprintf(out_buffer, buffer_size, written, "%s%s%s%u", pointer, vector, primitive, (uint32_t)id);
        }

        int32_t generateFunctionName(symbolid_t id, char* out_buffer, int32_t buffer_size, int32_t written)
        {
            return doSnprintf(out_buffer, buffer_size, written, "func_%u", (uint32_t)id);
        }

        int32_t generateOpenCLType(datatype_t dt, char* out_buffer, int32_t buffer_size, int32_t written)
        {
            const auto primitive_component = dt & DataType::PrimitiveMask;
            const auto vector_component = dt & DataType::ComponentMask;
            const auto pointer_component = dt & DataType::PointerMask;

            const char* primitive = nullptr;
            switch(primitive_component)
            {
                case DataType::Void:    primitive = "void"; break;
                case DataType::Char:    primitive = "char"; break;
                case DataType::UChar:   primitive = "uchar"; break;
                case DataType::Short:   primitive = "short"; break;
                case DataType::UShort:  primitive = "ushort"; break;
                case DataType::Int:     primitive = "int"; break;
                case DataType::UInt:    primitive = "uint"; break;
                case DataType::Long:    primitive = "long"; break;
                case DataType::ULong:   primitive = "ulong"; break;
                case DataType::Float:   primitive = "float"; break;
                case DataType::Double:  primitive = "double"; break;
            }
            SPARK_ASSERT(primitive != nullptr);

            const char* vector = nullptr;
            switch(vector_component)
            {
                case DataType::Vector2: vector = "2"; break;
                default:                vector = ""; break;
            }

            const char* pointer = nullptr;
            switch(pointer_component)
            {
                case DataType::Pointer: pointer = "*"; break;
                default:                pointer = ""; break;
            }

            return doSnprintf(out_buffer, buffer_size, written, "%s%s%s ", primitive, vector, pointer);
        }

        int32_t generateIndent(uint32_t indent, char* out_buffer, int32_t buffer_size, int32_t written)
        {
            for(uint32_t k = 0; k < indent; k++)
            {
                written = doSnprintf(out_buffer, buffer_size, written, "%s", "    ");
            }
            return written;
        }

        int32_t generateValueNode(Spark::Node* value, char* out_buffer, int32_t buffer_size, int32_t written)
        {
            return written;
        }

        int32_t generateControlNode(Spark::Node* control, uint32_t indent, char* out_buffer, int32_t buffer_size, int32_t written)
        {
            SPARK_ASSERT(control->_type == NodeType::Control);

            switch(control->_control)
            {
                case Control::If:
                    SPARK_ASSERT(control->_children.size() == 2);

                    break;
                case Control::ElseIf:
                    SPARK_ASSERT(control->_children.size() == 2);

                    break;
                case Control::Else:
                    SPARK_ASSERT(control->_children.size() == 1);

                    break;
                case Control::While:
                    SPARK_ASSERT(control->_children.size() == 2);

                    break;
                default:
                    SPARK_ASSERT(false);
            }

            return written;
        }

        int32_t generateScopeBlock(Spark::Node* scopeBlock, uint32_t indent, char* out_buffer, int32_t buffer_size, int32_t written)
        {
            SPARK_ASSERT(scopeBlock->_type == NodeType::ScopeBlock);
            written = generateIndent(indent, out_buffer, buffer_size, written);
            written = doSnprintf(out_buffer, buffer_size, written, "%s", "{\n");

            for(Node* currentNode : scopeBlock->_children)
            {
                written = generateIndent(indent + 1, out_buffer, buffer_size, written);

                switch(currentNode->_type)
                {
                    case NodeType::Control:
                    {
                        written = generateControlNode(currentNode, indent + 1, out_buffer, buffer_size, written);
                    }
                    default:
                        break;
                }

                written = doSnprintf(out_buffer, buffer_size, written, "%s", "\n");
            }


            written = generateIndent(indent, out_buffer, buffer_size, written);
            written = doSnprintf(out_buffer, buffer_size, written, "%s", "}\n\n");

            return written;
        }

        int32_t generateFunction(Spark::Node* node, char* out_buffer, int32_t buffer_size, int32_t written)
        {
            SPARK_ASSERT(node->_type == NodeType::Function);
            SPARK_ASSERT(node->_children.size() == 2);

            if(node->_function.entrypoint)
            {
                written = doSnprintf(out_buffer, buffer_size, written, "%s", "__kernel ");
            }

            // name and return type
            written = generateOpenCLType(node->_function.returnType, out_buffer, buffer_size, written);
            written = generateFunctionName(node->_function.id, out_buffer, buffer_size, written);

            // print parameter list
            written = doSnprintf(out_buffer, buffer_size, written, "%s", "(");
            Node* parameterList = node->_children.front();
            SPARK_ASSERT(parameterList->_type == NodeType::Control && parameterList->_control == Control::ParameterList);

            const auto paramCount = parameterList->_children.size();
            for(size_t k = 0; k < paramCount; k++)
            {
                if(k > 0)
                {
                    written = doSnprintf(out_buffer, buffer_size, written, "%s", ", ");
                }
                Node* currentChild = parameterList->_children[k];
                written = generateOpenCLType(currentChild->_symbol.type, out_buffer, buffer_size, written);
                written = generateSymbolName(currentChild->_symbol.id, currentChild->_symbol.type, out_buffer, buffer_size, written);
            }
            written = doSnprintf(out_buffer, buffer_size, written, "%s", ")\n");

            // function contents
            Node* functionBody = node->_children.back();
            written = generateScopeBlock(functionBody, 0, out_buffer, buffer_size, written);

            return written;
        }

        int32_t generateSource(Spark::Node* node, char* out_buffer, int32_t buffer_size)
        {
            SPARK_ASSERT(node->_type == NodeType::Control && node->_control == Control::Root);

            int32_t written = 0;
            for(Node* func : node->_children)
            {
                written = generateFunction(func, out_buffer, buffer_size, written);
            }

            return written;
        }
    }
}

int32_t spark_node_to_opencl(Spark::Node* node, char* out_buffer, int32_t buffer_size, spark_error_t** error)
{
    return Spark::Internal::TranslateExceptions(
        error,
        [&]
        {
            // +1 for null terminator
            return Spark::Internal::generateSource(node, out_buffer, buffer_size) + 1;
        });
}