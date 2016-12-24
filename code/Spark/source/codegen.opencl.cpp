#include "spark.hpp"

// spark internal
#include "node.hpp"
#include "text_utilities.hpp"
#include "error.hpp"

using std::string;
using std::unordered_set;

namespace Spark
{
    namespace Internal
    {
        struct opencl_data
        {
            int32_t indent = 0;
            unordered_set<symbolid_t> inited_variables;
        };

        using context = codegen_context<opencl_data>;

        // forward declare
        static void generateValueNode(context& ctx, Spark::Node* value);
        static void generateScopeBlock(context& ctx, Spark::Node* scopeBlock);

        static void generateSymbolName(context& ctx, symbolid_t id, datatype_t dt)
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

            doSnprintf(ctx, "%s%s%s%u", pointer, vector, primitive, (uint32_t)id);
        }

        static void generateFunctionName(context& ctx, symbolid_t id)
        {
            doSnprintf(ctx, "func_%u", (uint32_t)id);
        }

        static void generateOpenCLType(context& ctx, datatype_t dt)
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

            doSnprintf(ctx, "%s%s%s", primitive, vector, pointer);
        }

        static void generateIndent(context& ctx)
        {
            for(int32_t k = 0; k < ctx.indent; k++)
            {
                doSnprintf(ctx, "    ");
            }
        }

        static void generateOperatorNode(context& ctx, Spark::Node* value)
        {
            SPARK_ASSERT(value->_type == NodeType::Operator);

            auto op = value->_operator.id;
            if(op == Operator::Break)
            {
                SPARK_ASSERT(value->_children.size() == 0);
                doSnprintf(ctx, "break");
            }
            else if(op >= Operator::Negate && op <= Operator::Dereference)
            {
                SPARK_ASSERT(value->_children.size() == 1);
                const char* unary[] =
                {
                    "-",
                    "&",
                    "++",
                    "--",
                    "!",
                    "~",
                    "*",
                };
                doSnprintf(ctx, "(%s", unary[op - Operator::Negate]);
                generateValueNode(ctx, value->_children.front());
                doSnprintf(ctx, ")");
            }
            else if(op >= Operator::PostfixIncrement && op <= Operator::PostfixIncrement)
            {
                SPARK_ASSERT(value->_children.size() == 1);
                const char* unary[] =
                {
                    "++",
                    "--",
                };
                doSnprintf(ctx, "(");
                generateValueNode(ctx, value->_children.front());
                doSnprintf(ctx, "%s)", unary[op - Operator::PostfixIncrement]);
            }
            else if(op == Operator::Index)
            {
                SPARK_ASSERT(value->_children.size() == 2);
                generateValueNode(ctx, value->_children.front());
                doSnprintf(ctx, "[");
                generateValueNode(ctx, value->_children.back());
                doSnprintf(ctx, "]");
            }
            else if(op >= Operator::Add && op <= Operator::LeftShift)
            {
                SPARK_ASSERT(value->_children.size() == 2);
                const char* binary[] =
                {
                    "+",
                    "-",
                    "*",
                    "/",
                    "%",
                    ">",
                    "<",
                    ">=",
                    "<=",
                    "!=",
                    "==",
                    "&&",
                    "||",
                    "&",
                    "|",
                    "^",
                    ">>",
                    "<<",
                };
                doSnprintf(ctx, "(");
                generateValueNode(ctx, value->_children.front());
                doSnprintf(ctx, " %s ", binary[op - Operator::Add]);
                generateValueNode(ctx, value->_children.back());
                doSnprintf(ctx, ")");
            }
            else if(op == Operator::Assignment)
            {
                SPARK_ASSERT(value->_children.size() == 2);
                if(value->_children.front()->_type == NodeType::Symbol)
                {
                    const auto id = value->_children.front()->_symbol.id;
                    if(ctx.inited_variables.find(id) == ctx.inited_variables.end())
                    {
                        ctx.inited_variables.insert(id);
                        const auto type = value->_children.front()->_symbol.type;
                        generateOpenCLType(ctx, type);
                        doSnprintf(ctx, " ");
                    }
                }

                generateValueNode(ctx, value->_children.front());
                doSnprintf(ctx, " = ");
                generateValueNode(ctx, value->_children.back());
            }
            else if(op == Operator::Call)
            {
                SPARK_ASSERT(value->_children.size() >= 1);
                SPARK_ASSERT(value->_children.front()->_type == NodeType::Function);
                generateFunctionName(ctx, value->_children.front()->_function.id);
                doSnprintf(ctx, "(");
                for(size_t k = 1; k < value->_children.size(); k++)
                {
                    Node* currentChild = value->_children[k];
                    if(k > 1)
                    {
                       doSnprintf(ctx, ", ");
                    }
                    generateValueNode(ctx, currentChild);
                }
                doSnprintf(ctx, ")");
            }
            else if(op == Operator::Property)
            {
                SPARK_ASSERT(value->_children.size() == 2);
                generateValueNode(ctx, value->_children.front());

                SPARK_ASSERT(value->_children.back()->_type == NodeType::Property);
                char property[5];
                spark_property_to_str(value->_children.back()->_property.id, property, countof(property));
                doSnprintf(ctx, ".%s", property);
            }
            else if(op == Operator::Return)
            {
                if(value->_children.size() == 0)
                {
                    SPARK_ASSERT(value->_operator.type == DataType::Void);
                    doSnprintf(ctx, "return");
                }
                else
                {
                    SPARK_ASSERT(value->_children.size() == 1);
                    doSnprintf(ctx, "return ");
                    generateValueNode(ctx, value->_children.front());
                }
            }
            else if(op == Operator::Cast)
            {
                SPARK_ASSERT(value->_children.size() == 1);
                doSnprintf(ctx, "(");
                generateOpenCLType(ctx, value->_operator.type);
                doSnprintf(ctx, ")");

                generateValueNode(ctx, value->_children.front());
            }
        }

        static void generateSymbolNode(context& ctx, Spark::Node* symbol)
        {
            generateSymbolName(ctx, symbol->_symbol.id, symbol->_symbol.type);
        }

        static void generateConstantNode(context& ctx, Spark::Node* constant)
        {
            SPARK_ASSERT(constant->_type == NodeType::Constant);
            void* raw = constant->_constant.buffer;
            switch(constant->_constant.type)
            {
                case DataType::Char:
                    doSnprintf(ctx, "%lli", *reinterpret_cast<int8_t*>(raw));
                    break;
                case DataType::UChar:
                    doSnprintf(ctx, "%lluu", *reinterpret_cast<uint8_t*>(raw));
                    break;
                case DataType::Short:
                    doSnprintf(ctx, "%lli", *reinterpret_cast<int16_t*>(raw));
                    break;
                case DataType::UShort:
                    doSnprintf(ctx, "%lluu", *reinterpret_cast<uint16_t*>(raw));
                    break;
                case DataType::Int:
                    doSnprintf(ctx, "%lli", *reinterpret_cast<int32_t*>(raw));
                    break;
                case DataType::UInt:
                    doSnprintf(ctx, "%lluu", *reinterpret_cast<uint32_t*>(raw));
                    break;
                case DataType::Long:
                    doSnprintf(ctx, "%lli", *reinterpret_cast<int64_t*>(raw));
                    break;
                case DataType::ULong:
                    doSnprintf(ctx, "%lluu", *reinterpret_cast<uint64_t*>(raw));
                    break;
                case DataType::Float:
                    doSnprintf(ctx, "%.9ef", *reinterpret_cast<float*>(raw));
                    break;
                case DataType::Double:
                    doSnprintf(ctx, "%.17e", *reinterpret_cast<double*>(raw));
                    break;
                default:
                    SPARK_ASSERT(!!"Invalid Constant Datatype");
            }
        }

        static void generateVectorNode(context& ctx, Spark::Node* vector)
        {
            SPARK_ASSERT(vector->_type == NodeType::Vector);
            datatype_t type = vector->_vector.type;
            if(type & DataType::Vector2)
            {
                SPARK_ASSERT(vector->_children.size() == 2);
                doSnprintf(ctx, "(");
                generateOpenCLType(ctx, type);
                doSnprintf(ctx, ")(");
                for(size_t k = 0; k < vector->_children.size(); k++)
                {
                    if(k > 0)
                    {
                        doSnprintf(ctx, ", ");
                    }
                    generateValueNode(ctx, vector->_children[k]);
                }
                doSnprintf(ctx, ")");
            }
        }

        static void generateValueNode(context& ctx, Spark::Node* value)
        {
            switch(value->_type)
            {
                case NodeType::Operator:
                    generateOperatorNode(ctx, value);
                    break;
                case NodeType::Symbol:
                    generateSymbolNode(ctx, value);
                    break;
                case NodeType::Constant:
                    generateConstantNode(ctx, value);
                    break;
                case NodeType::Vector:
                    generateVectorNode(ctx, value);
                    break;
                default:
                    SPARK_ASSERT(value->_type == NodeType::Operator ||
                                 value->_type == NodeType::Symbol ||
                                 value->_type == NodeType::Constant ||
                                 value->_type == NodeType::Vector);
            }
        }

        static void generateControlNode(context& ctx, Spark::Node* control)
        {
            SPARK_ASSERT(control->_type == NodeType::Control);

            switch(control->_control)
            {
                case Control::If:
                    SPARK_ASSERT(control->_children.size() == 2);
                    doSnprintf(ctx, "if (");
                    generateValueNode(ctx, control->_children.front());
                    doSnprintf(ctx, ")\n");
                    generateScopeBlock(ctx, control->_children.back());
                    break;
                case Control::ElseIf:
                    SPARK_ASSERT(control->_children.size() == 2);
                    doSnprintf(ctx, "else if (");
                    generateValueNode(ctx, control->_children.front());
                    doSnprintf(ctx, ")\n");
                    generateScopeBlock(ctx, control->_children.back());
                    break;
                case Control::Else:
                    SPARK_ASSERT(control->_children.size() == 1);
                    doSnprintf(ctx, "else\n");
                    generateScopeBlock(ctx, control->_children.back());
                    break;
                case Control::While:
                {
                    SPARK_ASSERT(control->_children.size() == 2);
                    doSnprintf(ctx, "while (");
                    generateValueNode(ctx, control->_children.front());
                    doSnprintf(ctx, ")\n");
                    generateScopeBlock(ctx, control->_children.back());
                    break;
                }
                default:
                    SPARK_ASSERT(false);
            }
        }

        static void generateScopeBlock(context& ctx, Spark::Node* scopeBlock)
        {
            SPARK_ASSERT(scopeBlock->_type == NodeType::ScopeBlock);

            generateIndent(ctx);
            doSnprintf(ctx, "{\n");

            ctx.indent += 1;
            for(Node* currentNode : scopeBlock->_children)
            {
                generateIndent(ctx);

                switch(currentNode->_type)
                {
                    case NodeType::Control:
                    {
                        generateControlNode(ctx, currentNode);
                        break;
                    }
                    case NodeType::Operator:
                    case NodeType::Symbol:
                    case NodeType::Constant:
                    case NodeType::Vector:
                        generateValueNode(ctx, currentNode);
                        doSnprintf(ctx, ";\n");
                        break;
                    case NodeType::Comment:
                        doSnprintf(ctx, "// %s\n", currentNode->_comment);
                        break;
                    default:
                        doSnprintf(ctx, "#unimplemented\n");
                        break;
                }
            }
            ctx.indent -= 1;

            generateIndent(ctx);
            doSnprintf(ctx, "}\n");
        }

        static void generateFunction(context& ctx, Spark::Node* node)
        {
            SPARK_ASSERT(node->_type == NodeType::Function);
            SPARK_ASSERT(node->_children.size() == 2);

            if(node->_function.entrypoint)
            {
                doSnprintf(ctx, "__kernel ");
            }

            // name and return type
            generateOpenCLType(ctx, node->_function.returnType);
            doSnprintf(ctx, " ");
            generateFunctionName(ctx, node->_function.id);

            // print parameter list
            doSnprintf(ctx, "(");
            Node* parameterList = node->_children.front();
            SPARK_ASSERT(parameterList->_type == NodeType::Control && parameterList->_control == Control::ParameterList);

            const auto paramCount = parameterList->_children.size();
            for(size_t k = 0; k < paramCount; k++)
            {
                if(k > 0)
                {
                    doSnprintf(ctx, ", ");
                }
                Node* currentChild = parameterList->_children[k];
                SPARK_ASSERT(currentChild->_type == NodeType::Symbol);
                // add to our set of init'd variables
                ctx.inited_variables.insert(currentChild->_symbol.id);

                generateOpenCLType(ctx, currentChild->_symbol.type);
                doSnprintf(ctx, " ");
                generateSymbolName(ctx, currentChild->_symbol.id, currentChild->_symbol.type);
            }
            doSnprintf(ctx, ")\n");

            // function contents
            Node* functionBody = node->_children.back();
            generateScopeBlock(ctx, functionBody);
        }

        static void generateSource(context& ctx, Spark::Node* node)
        {
            SPARK_ASSERT(node->_type == NodeType::Control && node->_control == Control::Root);
            for(Node* func : node->_children)
            {
                generateFunction(ctx, func);
            }
        }
    }
}

int32_t spark_node_to_opencl(Spark::Node* node, char* out_buffer, int32_t buffer_size, spark_error_t** error)
{
    return Spark::Internal::TranslateExceptions(
        error,
        [&]
        {
            Spark::Internal::context ctx;
            {
                ctx.buffer = out_buffer;
                ctx.capacity = buffer_size;
            }

            Spark::Internal::generateSource(ctx, node);

            // +1 for null terminator
            return ctx.written + 1;
        });
}