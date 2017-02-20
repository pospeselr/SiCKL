#include "spark.hpp"

// spark internal
#include "node.hpp"
#include "error.hpp"
#include "text_utilities.hpp"

using std::string;
using std::unordered_set;

using namespace spark;
using namespace spark::lib;
using namespace spark::shared;

namespace spark
{
    namespace lib
    {
        struct opencl_data
        {
            int32_t indent = 0;
            unordered_set<spark_symbolid_t> inited_variables;
        };

        using context = codegen_context<opencl_data>;

        // forward declare
        static void generateValueNode(context& ctx, spark_node_t* value);
        static void generateScopeBlock(context& ctx, spark_node_t* scopeBlock);

        static void generateSymbolName(context& ctx, spark_symbolid_t id, Datatype dt)
        {
            const auto primitive = dt.GetPrimitive();
            const auto components = dt.GetComponents();
            const auto pointer = dt.GetPointer();

            const char* primitive_str = nullptr;
            switch(primitive)
            {
                case Primitive::Char:    primitive_str = "s8_"; break;
                case Primitive::UChar:   primitive_str = "u8_"; break;
                case Primitive::Short:   primitive_str = "s16_"; break;
                case Primitive::UShort:  primitive_str = "u16_"; break;
                case Primitive::Int:     primitive_str = "s32_"; break;
                case Primitive::UInt:    primitive_str = "u32_"; break;
                case Primitive::Long:    primitive_str = "s64_"; break;
                case Primitive::ULong:   primitive_str = "u64_"; break;
                case Primitive::Float:   primitive_str = "flt_"; break;
                case Primitive::Double:  primitive_str = "dbl_"; break;
                default:
                    SPARK_ASSERT(false);
            }

            const char* components_str = nullptr;
            switch(components)
            {
                case Components::Vector2: components_str = "vec2_"; break;
                default:                  components_str = ""; break;
            }

            const char* pointer_str = pointer ? "p_" : "";

            doSnprintf(ctx, "%s%s%s%u", pointer_str, components_str, primitive_str, (uint32_t)id);
        }

        static void generateFunctionName(context& ctx, spark_symbolid_t id)
        {
            doSnprintf(ctx, "func_%u", (uint32_t)id);
        }

        static void generateOpenCLType(context& ctx, Datatype dt)
        {
            const auto primitive = dt.GetPrimitive();
            const auto components = dt.GetComponents();
            const auto pointer = dt.GetPointer();

            const char* primitive_str = nullptr;
            switch(primitive)
            {
                case Primitive::Void:    primitive_str = "void"; break;
                case Primitive::Char:    primitive_str = "char"; break;
                case Primitive::UChar:   primitive_str = "uchar"; break;
                case Primitive::Short:   primitive_str = "short"; break;
                case Primitive::UShort:  primitive_str = "ushort"; break;
                case Primitive::Int:     primitive_str = "int"; break;
                case Primitive::UInt:    primitive_str = "uint"; break;
                case Primitive::Long:    primitive_str = "long"; break;
                case Primitive::ULong:   primitive_str = "ulong"; break;
                case Primitive::Float:   primitive_str = "float"; break;
                case Primitive::Double:  primitive_str = "double"; break;
                default:
                    SPARK_ASSERT(false);
            }

            const char* components_str = nullptr;
            switch(components)
            {
                case Components::Vector2: components_str = "2"; break;
                default:                components_str = ""; break;
            }

            const char* pointer_str = pointer ? "*" : "";

            doSnprintf(ctx, "%s%s%s", primitive_str, components_str, pointer_str);
        }

        static void generateIndent(context& ctx)
        {
            for(int32_t k = 0; k < ctx.indent; k++)
            {
                doSnprintf(ctx, "    ");
            }
        }

        static void generateOperatorNode(context& ctx, spark_node_t* value)
        {
            SPARK_ASSERT(value->_type == spark_nodetype::operation);

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
                doSnprintf(ctx, "(%s", unary[static_cast<spark_operator_t>(op - Operator::Negate)]);
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
                doSnprintf(ctx, "%s)", unary[static_cast<spark_operator_t>(op - Operator::PostfixIncrement)]);
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
                doSnprintf(ctx, " %s ", binary[static_cast<spark_operator_t>(op - Operator::Add)]);
                generateValueNode(ctx, value->_children.back());
                doSnprintf(ctx, ")");
            }
            else if(op == Operator::Assignment)
            {
                SPARK_ASSERT(value->_children.size() == 2);
                if(value->_children.front()->_type == spark_nodetype::symbol)
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
                SPARK_ASSERT(value->_children.front()->_type == spark_nodetype::function);
                generateFunctionName(ctx, value->_children.front()->_function.id);
                doSnprintf(ctx, "(");
                for(size_t k = 1; k < value->_children.size(); k++)
                {
                    auto* currentChild = value->_children[k];
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

                SPARK_ASSERT(value->_children.back()->_type == spark_nodetype::property);
                char property[5];
                spark_property_to_str(static_cast<spark_property_t>(value->_children.back()->_property.id), property, countof(property));
                doSnprintf(ctx, ".%s", property);
            }
            else if(op == Operator::Return)
            {
                if(value->_children.size() == 0)
                {
                    SPARK_ASSERT(value->_operator.type.GetPrimitive() == Primitive::Void);
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

        static void generateSymbolNode(context& ctx, spark_node_t* symbol)
        {
            generateSymbolName(ctx, symbol->_symbol.id, symbol->_symbol.type);
        }

        static void generateConstantNode(context& ctx, spark_node_t* constant)
        {
            SPARK_ASSERT(constant->_type == spark_nodetype::constant);
            auto dt = constant->_constant.type;
            SPARK_ASSERT(dt.GetComponents() == Components::Scalar);

            void* raw = constant->_constant.buffer;
            switch(constant->_constant.type.GetPrimitive())
            {
                case Primitive::Char:
                    doSnprintf(ctx, "%lli", *reinterpret_cast<int8_t*>(raw));
                    break;
                case Primitive::UChar:
                    doSnprintf(ctx, "%lluu", *reinterpret_cast<uint8_t*>(raw));
                    break;
                case Primitive::Short:
                    doSnprintf(ctx, "%lli", *reinterpret_cast<int16_t*>(raw));
                    break;
                case Primitive::UShort:
                    doSnprintf(ctx, "%lluu", *reinterpret_cast<uint16_t*>(raw));
                    break;
                case Primitive::Int:
                    doSnprintf(ctx, "%lli", *reinterpret_cast<int32_t*>(raw));
                    break;
                case Primitive::UInt:
                    doSnprintf(ctx, "%lluu", *reinterpret_cast<uint32_t*>(raw));
                    break;
                case Primitive::Long:
                    doSnprintf(ctx, "%lli", *reinterpret_cast<int64_t*>(raw));
                    break;
                case Primitive::ULong:
                    doSnprintf(ctx, "%lluu", *reinterpret_cast<uint64_t*>(raw));
                    break;
                case Primitive::Float:
                    doSnprintf(ctx, "%.9ef", *reinterpret_cast<float*>(raw));
                    break;
                case Primitive::Double:
                    doSnprintf(ctx, "%.17e", *reinterpret_cast<double*>(raw));
                    break;
                default:
                    SPARK_ASSERT(!!"Invalid Constant Datatype");
            }
        }

        static void generateVectorNode(context& ctx, spark_node_t* vector)
        {
            SPARK_ASSERT(vector->_type == spark_nodetype::vector);
            auto type = vector->_vector.type;
            auto components = type.GetComponents();
            if(components == Components::Vector2)
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

        static void generateValueNode(context& ctx, spark_node_t* value)
        {
            switch(value->_type)
            {
                case spark_nodetype::operation:
                    generateOperatorNode(ctx, value);
                    break;
                case spark_nodetype::symbol:
                    generateSymbolNode(ctx, value);
                    break;
                case spark_nodetype::constant:
                    generateConstantNode(ctx, value);
                    break;
                case spark_nodetype::vector:
                    generateVectorNode(ctx, value);
                    break;
                default:
                    SPARK_ASSERT(value->_type == spark_nodetype::operation ||
                                 value->_type == spark_nodetype::symbol ||
                                 value->_type == spark_nodetype::constant ||
                                 value->_type == spark_nodetype::vector);
            }
        }

        static void generateControlNode(context& ctx, spark_node_t* control)
        {
            SPARK_ASSERT(control->_type == spark_nodetype::control);

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

        static void generateScopeBlock(context& ctx, spark_node_t* scopeBlock)
        {
            SPARK_ASSERT(scopeBlock->_type == spark_nodetype::scope_block);

            generateIndent(ctx);
            doSnprintf(ctx, "{\n");

            ctx.indent += 1;
            for(auto currentNode : scopeBlock->_children)
            {
                generateIndent(ctx);

                switch(currentNode->_type)
                {
                    case spark_nodetype::control:
                    {
                        generateControlNode(ctx, currentNode);
                        break;
                    }
                    case spark_nodetype::operation:
                    case spark_nodetype::symbol:
                    case spark_nodetype::constant:
                    case spark_nodetype::vector:
                        generateValueNode(ctx, currentNode);
                        doSnprintf(ctx, ";\n");
                        break;
                    case spark_nodetype::comment:
                        doSnprintf(ctx, "/* %s */\n", currentNode->_comment);
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

        static void generateFunction(context& ctx, spark_node_t* node)
        {
            SPARK_ASSERT(node->_type == spark_nodetype::function);
            SPARK_ASSERT(node->_children.size() == 2);

            // leave empty line between functions
            doSnprintf(ctx, "\n");

            // name and return type
            if(node->_function.entrypoint)
            {
                doSnprintf(ctx, "__kernel ");
                generateOpenCLType(ctx, node->_function.returnType);
                // entry point always called main
                doSnprintf(ctx, " main");
            }
            else
            {
                generateOpenCLType(ctx, node->_function.returnType);
                doSnprintf(ctx, " ");
                generateFunctionName(ctx, node->_function.id);
            }

            // print parameter list
            doSnprintf(ctx, "(");
            auto parameterList = node->_children.front();
            SPARK_ASSERT(parameterList->_type == spark_nodetype::control && parameterList->_control == Control::ParameterList);

            const auto paramCount = parameterList->_children.size();
            for(size_t k = 0; k < paramCount; k++)
            {
                if(k > 0)
                {
                    doSnprintf(ctx, ", ");
                }
                auto currentChild = parameterList->_children[k];
                SPARK_ASSERT(currentChild->_type == spark_nodetype::symbol);
                // add to our set of init'd variables
                ctx.inited_variables.insert(currentChild->_symbol.id);

                if(currentChild->_symbol.type.GetPointer())
                {
                    doSnprintf(ctx, "__global ");
                }

                generateOpenCLType(ctx, currentChild->_symbol.type);
                doSnprintf(ctx, " ");
                generateSymbolName(ctx, currentChild->_symbol.id, currentChild->_symbol.type);
            }
            doSnprintf(ctx, ")\n");

            // function contents
            auto functionBody = node->_children.back();
            generateScopeBlock(ctx, functionBody);
        }

        int32_t generateOpenCLSource(spark_node_t* node, char* out_buffer, int32_t buffer_size)
        {
            SPARK_ASSERT(node->_type == spark_nodetype::control && node->_control == Control::Root);

            context ctx;
            {
                ctx.buffer = out_buffer;
                ctx.capacity = buffer_size;
            }

            // generate all the functions
            for(auto func : node->_children)
            {
                generateFunction(ctx, func);
            }

            // +1 for null terminator
            return ctx.written + 1;
        }
    }
}

SPARK_EXPORT int32_t spark_node_to_opencl(spark_node_t* node, char* out_buffer, int32_t buffer_size, spark_error_t** error)
{
    return TranslateExceptions(
        error,
        [&]
        {
            return generateOpenCLSource(node, out_buffer, buffer_size);
        });
}