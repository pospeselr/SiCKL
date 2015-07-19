#include "SiCKL.h"

#undef If
#undef ElseIf
#undef Else
#undef EndIf
#undef While
#undef ForInRange
#undef Return
namespace SiCKL
{
    namespace Internal
    {
        //
        // Structs
        //

        template<uint32_t SIZE>
        struct CharStack
        {
            CharStack()
                : _count(0)
            { }

            void push(char c)
            {
                SICKL_ASSERT(_count < SIZE);

                _buffer[_count++] = c;
            }

            char pop()
            {
                SICKL_ASSERT(_count > 0);

                return _buffer[--_count];
            }

            uint32_t count() const
            {
                return _count;
            }

        private:
            char _buffer[SIZE];
            uint32_t _count;
        };

        const char newline = '\n';
        struct StringBuffer
        {
            StringBuffer()
            {
                const size_t count = 64;
                _buffer = new char[count];
                SICKL_ASSERT(_buffer != nullptr);
                _allocated = count;
                ::memset(_buffer, 0x00, count);
                _length = 0;
                _tail = _buffer;
            }

            ~StringBuffer()
            {
                delete[] _buffer;
                _buffer = nullptr;
                _tail = nullptr;
                _length = 0;
                _allocated = 0;
            }

            sickl_int Clear()
            {
                ::memset(_buffer, 0x00, _length);
                _length = 0;
                _tail = _buffer;

                return SICKL_SUCCESS;
            }

            StringBuffer& operator<<(const char* in_string)
            {
                while(*in_string)
                {
                    if(_length == (_allocated - 1))
                    {
                        GrowBuffer();
                    }
                    *_tail = *in_string;
                    ++_tail;
                    ++_length;
                    ++in_string;
                }
                return *this;
            }

            StringBuffer& operator<<(const char in_char)
            {
                if(_length == (_allocated - 1))
                {
                    GrowBuffer();
                }
                *_tail = in_char;
                ++_tail;
                ++_length;
                return *this;
            }

            // prints the name of a symbol id
            StringBuffer& operator<<(const symbol_id_t& in_id)
            {
                SICKL_ASSERT(in_id > 0);
                const int id = (in_id - 1);
            
                CharStack<8> stack;

                // convert from number to ASCII letter
                auto letter = [](int32_t i) -> char
                {
                    return char('a' + i);
                };

                int x = id;

                stack.push(letter(x % 26));
                x = x - x % 26;

                while(x > 0)
                {
                    x = x / 26;
                    stack.push(letter((x - 1) % 26));
                    if((x % 26) == 0)
                    {
                        x = x - 26;
                    }
                    else
                    {
                        x -= x - x % 26;
                    }
                }

                while(stack.count())
                {
                    *this << stack.pop();
                }
                return *this;
            }

            StringBuffer& operator<<(const ReturnType_t type)
            {
                switch(type & ~(ReturnType::Buffer1D | ReturnType::Buffer2D))
                {
                case ReturnType::Void:
                    *this << "void";
                    break;
                case ReturnType::Bool:
                    *this << "bool";
                    break;
                case ReturnType::Int:
                    *this << "int";
                    break;
                case ReturnType::Int2:
                    *this << "int2";
                    break;
                case ReturnType::Int3:
                    *this << "int3";
                    break;
                case ReturnType::Int4:
                    *this << "int4";
                    break;
                case ReturnType::UInt:
                    *this << "uint";
                    break;
                case ReturnType::UInt2:
                    *this << "uint2";
                    break;
                case ReturnType::UInt3:
                    *this << "uint3";
                    break;
                case ReturnType::UInt4:
                    *this << "uint4";
                    break;
                case ReturnType::Float:
                    *this << "float";
                    break;
                case ReturnType::Float2:
                    *this << "float2";
                    break;
                case ReturnType::Float3:
                    *this << "float3";
                    break;
                case ReturnType::Float4:
                    *this << "float4";
                    break;
                default:
                    SICKL_ASSERT(false);
                }

                // if it's a buffer add pointer char
                if(type & (ReturnType::Buffer1D | ReturnType::Buffer2D))
                {
                    *this << "*";
                }

                return *this;
            }

            StringBuffer& operator<<(const bool b)
            {
                *this << (b ? "true" : "false");
                return *this;
            }

            StringBuffer& operator<<(const int32_t i)
            {
                char buffer[12];
                int result = sprintf(buffer, "%i", i);
                SICKL_ASSERT(result >= 0 && size_t(result) < count_of(buffer));
                *this << buffer;

                return *this;
            }

            StringBuffer& operator<<(const uint32_t u)
            {
                char buffer[11];
                int result = sprintf(buffer, "%u", u);
                SICKL_ASSERT(result >= 0 && size_t(result) < count_of(buffer));
                *this << buffer << 'u';
                return *this;
            }

            StringBuffer& operator<<(const float f)
            {
                char buffer[24] = {0};
                int result = sprintf(buffer, "%.9e", f);
                SICKL_ASSERT(result >= 0 && size_t(result) < count_of(buffer));
                *this << buffer << 'f';
                return *this;
            }

            operator const char*() const
            {
                return _buffer;
            }

        private:
            void GrowBuffer()
            {
                SICKL_ASSERT(_length == (_allocated - 1));

                const size_t old_count = _allocated;
                const size_t new_count = old_count * 2;
                char* new_buffer = new char[new_count];
                SICKL_ASSERT(new_buffer != nullptr);

                ::memcpy(new_buffer, _buffer, old_count);
                ::memset(new_buffer + old_count, 0x00, old_count);

                delete[] _buffer;
                _buffer = new_buffer;
                _allocated = new_count;
                _tail = new_buffer + old_count - 1;
            }

            // head of our string
            char* _buffer;
            // our null terminator
            char* _tail;
            size_t _length;
            size_t _allocated;
        };

        //
        // Data
        //

        static StringBuffer sb;
        static uint32_t indent;
        static std::set<symbol_id_t> declared_vars;

        //
        // Functions
        //

        // forward declare

        sickl_int print_indent();
        sickl_int print_code(const ASTNode* node);
        sickl_int print_newline(const ASTNode* node);
        sickl_int print_unary_operator(const ASTNode* node);
        sickl_int print_binary_operator(const ASTNode* node);
        sickl_int print_line_statements(const ASTNode* node);
        sickl_int print_builtin_function(const ASTNode* node);
        sickl_int print_function(const ASTNode* node);
        sickl_int print_kernel_source(const ASTNode* in_root);

        sickl_int print_indent()
        {
            for(size_t i = 0; i < indent; i++)
            {
                sb << ' ';
            }
            return SICKL_SUCCESS;
        }

        sickl_int print_newline(const ASTNode* node)
        {
            switch(node->_node_type)
            {
            default:
                sb << ';' << newline;
            case NodeType::If:
            case NodeType::ElseIf:
            case NodeType::Else:
            case NodeType::While:
            case NodeType::ForInRange:
                break;
            }

            return SICKL_SUCCESS;
        }

        sickl_int print_unary_operator(const ASTNode* node)
        {
            SICKL_ASSERT(node->_count == 1);
             switch(node->_node_type)
             {
             case NodeType::LogicalNot:
                 sb << "!( "; break;
             case NodeType::BitwiseNot:
                 sb << "~( "; break;
             case NodeType::UnaryMinus:
                 sb << "-( "; break;
             default:
                SICKL_ASSERT(!"Invalid ASTNode given to print_unary_operator")
             }

             ReturnIfError(print_code(node->_children[0]));
             sb << ')';

            return SICKL_SUCCESS;
        }

        sickl_int print_binary_operator(const ASTNode* node)
        {
            SICKL_ASSERT(node->_count == 2);

            sb << '(';
            ReturnIfError(print_code(node->_children[0]));

            switch(node->_node_type)
            {
            case NodeType::Equal:
                sb << " == "; break;
            case NodeType::NotEqual:
                sb << " != "; break;
            case NodeType::Greater:
                sb << " > "; break;
            case NodeType::GreaterEqual:
                sb << " >= "; break;
            case NodeType::Less:
                sb << " < "; break;
            case NodeType::LessEqual:
                sb << " <= "; break;
            case NodeType::LogicalAnd:
                sb << " && "; break;
            case NodeType::LogicalOr:
                sb << " || "; break;
            case NodeType::BitwiseAnd:
                sb << " & "; break;
            case NodeType::BitwiseOr:
                sb << " | "; break;
            case NodeType::BitwiseXor:
                sb << " ^ "; break;
            case NodeType::LeftShift:
                sb << " << "; break;
            case NodeType::RightShift:
                sb << " >> "; break;
            case NodeType::Add:
                sb << " + "; break;
            case NodeType::Subtract:
                sb << " - "; break;
            case NodeType::Multiply:
                sb << " * "; break;
            case NodeType::Divide:
                sb << " / "; break;
            case NodeType::Modulo:
                sb << " % "; break;
            default:
                SICKL_ASSERT(false);
            }

            ReturnIfError(print_code(node->_children[1]));
            sb << ')';
            return SICKL_SUCCESS;
        }

        sickl_int print_line_statements(size_t offset, const ASTNode* node)
        {
            for(size_t i = offset; i < node->_count; ++i)
            {
                ReturnIfError(print_indent());
                ReturnIfError(print_code(node->_children[i]));
                ReturnIfError(print_newline(node->_children[i]));
            }

            return SICKL_SUCCESS;
        }

        sickl_int print_builtin_function(const ASTNode* node)
        {
            SICKL_ASSERT(node->_children[0]->_node_type == NodeType::Literal);
            SICKL_ASSERT(node->_children[0]->_return_type == ReturnType::Int);
            const int32_t func_id = (*(int32_t*)node->_children[0]->_literal.data);

            switch(func_id)
            {
            case BuiltinFunction::Index:
                sb << "(uint3)(get_global_id(0), get_global_id(1), get_global_id(2))";
                break;
            case BuiltinFunction::NormalizedIndex:
                sb << "(float3)((float)(get_global_id(0) + 1) / (float)(get_global_size(0)), (float)(get_global_id(1) + 1) / (float)(get_global_size(1)), (float)(get_global_id(2) + 1) / (float)(get_global_size(2)))";
                break;
            case BuiltinFunction::BufferLength:
                SICKL_ASSERT(node->_count == 2);
                SICKL_ASSERT(node->_children[1]->_return_type & ReturnType::Buffer1D);
                sb << node->_children[1]->_sid << "_length";
                break;
            case BuiltinFunction::BufferWidth:
                SICKL_ASSERT(node->_count == 2);
                SICKL_ASSERT(node->_children[1]->_return_type & ReturnType::Buffer2D);
                sb << node->_children[1]->_sid << "_width";
                break;
            case BuiltinFunction::BufferHeight:
                SICKL_ASSERT(node->_count == 2);
                SICKL_ASSERT(node->_children[1]->_return_type & ReturnType::Buffer2D);
                sb << node->_children[1]->_sid << "_height";
                break;
            default:
                const char* function_names[] =
                {
                    nullptr,
                    nullptr,
                    "sin",
                    "cos",
                    "tan",
                    "asin",
                    "acos",
                    "atan",
                    "sinh",
                    "cosh",
                    "tanh",
                    "asinh",
                    "acosh",
                    "atanh",
                    "pow",
                    "exp",
                    "log",
                    "exp2",
                    "log2",
                    "sqrt",
                    "abs",
                    "sign",
                    "floor",
                    "ceil",
                    "min",
                    "max",
                    "clamp",
                    "isnan",
                    "isinf",
                    "length",
                    "distance",
                    "dot",
                    "cross",
                    "normalize",
                };
                sb << function_names[func_id] << '(';
                for(size_t i = 1; i < node->_count; ++i)
                {
                    if(i != 1)
                    {
                        sb << ',';
                    }
                    ReturnIfError(print_code(node->_children[i]));
                }
                sb << ')';
                break;
            }

            return SICKL_SUCCESS;
        }

        sickl_int print_code(const ASTNode* node)
        {
            switch(node->_node_type)
            {

            //
            // User-Function
            //
            case NodeType::Body:
                SICKL_ASSERT(indent == 0);
                indent++;

                ReturnIfError(print_line_statements(0, node));

                break;
            case NodeType::Return:
                SICKL_ASSERT(node->_count == 1);
                sb << "return ";
                ReturnIfError(print_code(node->_children[0]));
                break;
            //
            // Control Flow
            //                
            case NodeType::If:
                SICKL_ASSERT(node->_count >= 1);
                sb << "if(";
                // conditional
                ReturnIfError(print_code(node->_children[0]));
                sb << ')' << newline;
                ReturnIfError(print_indent());
                sb << '{' << newline;
                indent++;

                // contents
                ReturnIfError(print_line_statements(1, node));

                indent--;
                ReturnIfError(print_indent());
                sb << '}' << newline;

                break;
            case NodeType::ElseIf:
                SICKL_ASSERT(node->_count >= 1);
                sb << "else if(";
                // conditional
                ReturnIfError(print_code(node->_children[0]));
                sb << ')' << newline;
                ReturnIfError(print_indent());
                sb << '{' << newline;
                indent++;

                // contents
                ReturnIfError(print_line_statements(1, node));

                indent--;
                ReturnIfError(print_indent());
                sb << '}' << newline;

                break;
            case NodeType::Else:
                sb << "else" << newline;
                ReturnIfError(print_indent());
                sb << '{' << newline;
                indent++;

                // contents
                ReturnIfError(print_line_statements(0, node));

                indent--;
                ReturnIfError(print_indent());
                sb << '}' << newline;

                break;
            case NodeType::While:
                SICKL_ASSERT(node->_count >= 1);
                sb << "while(";
                // conditional
                ReturnIfError(print_code(node->_children[0]));
                sb << ')' << newline;
                ReturnIfError(print_indent());
                sb << '{' << newline;
                indent++;

                // contents
                ReturnIfError(print_line_statements(1, node));

                indent--;
                ReturnIfError(print_indent());
                sb << '}' << newline;

                break;
            case NodeType::ForInRange:
                SICKL_ASSERT(node->_count >= 3)
                SICKL_ASSERT(node->_children[0]->_return_type == ReturnType::Int);
                SICKL_ASSERT(node->_children[1]->_node_type == NodeType::Literal);
                SICKL_ASSERT(node->_children[1]->_return_type == ReturnType::Int);
                SICKL_ASSERT(node->_children[2]->_node_type == NodeType::Literal);
                SICKL_ASSERT(node->_children[2]->_return_type == ReturnType::Int);

                sb << "for (int " << node->_children[0]->_sid << '=' << *(int32_t*)node->_children[1]->_literal.data << ';';
                sb << node->_children[0]->_sid << '<' << *(int32_t*)node->_children[2]->_literal.data << ';';
                sb << "++" << node->_children[0]->_sid << ')' << newline;

                ReturnIfError(print_indent());
                sb << '{' << newline;
                indent++;

                // contents
                ReturnIfError(print_line_statements(3, node));

                indent--;
                ReturnIfError(print_indent());
                sb << '}' << newline;

                break;
            //
            // Variables
            //
            case NodeType::Var:
                sb << node->_sid;
                break;
            case NodeType::Literal:
                switch(node->_return_type)
                {
                case ReturnType::Bool:
                    sb << *(bool*)node->_literal.data;
                    break;
                case ReturnType::Int:
                    sb << *(cl_int*)node->_literal.data;
                    break;
                case ReturnType::UInt:
                    sb << *(cl_uint*)node->_literal.data;
                    break;
                case ReturnType::Float:
                    sb << *(cl_float*)node->_literal.data;
                    break;
                default:
                    // invalid Literal type
                    SICKL_ASSERT(false);
                }
                break;
            case NodeType::Member:
                {
                    SICKL_ASSERT(node->_count == 2);
                    switch(node->_children[0]->_return_type)
                    {
                    case ReturnType::Int2:
                    case ReturnType::UInt2:
                    case ReturnType::Float2:
                    case ReturnType::Int3:
                    case ReturnType::UInt3:
                    case ReturnType::Float3:
                    case ReturnType::Int4:
                    case ReturnType::UInt4:
                    case ReturnType::Float4:
                        break;
                    default:
                        SICKL_ASSERT(false);
                    }
                    SICKL_ASSERT(node->_children[1]->_node_type == NodeType::Literal);

                    ReturnIfError(print_code(node->_children[0]));

                    uint32_t index = *(int32_t*)node->_children[1]->_literal.data;
                    SICKL_ASSERT(index < 4);

                    const char* vector_components[] = {".s0", ".s1", ".s2", ".s3"};
                    sb << vector_components[index];
                }
                break;
            //
            // Operators
            //
            case NodeType::Assignment:
                {
                    SICKL_ASSERT(node->_count == 2);
                    // see if we need to specify the type here
                    if(node->_children[0]->_node_type == NodeType::Var &&
                       declared_vars.find(node->_children[0]->_sid) == declared_vars.end())
                    {
                        sb << node->_children[0]->_return_type << ' ';
                        declared_vars.insert(node->_children[0]->_sid);
                    }

                    // var name
                    ReturnIfError(print_code(node->_children[0]));
                    sb << " = ";
                    // values
                    ReturnIfError(print_code(node->_children[1]));
                }
                break;
            // Comparison
            case NodeType::Equal:
            case NodeType::NotEqual:
            case NodeType::Greater:
            case NodeType::GreaterEqual:
            case NodeType::Less:
            case NodeType::LessEqual:
            // Boolean
            case NodeType::LogicalAnd:
            case NodeType::LogicalOr:
            // Bitwise
            case NodeType::BitwiseAnd:
            case NodeType::BitwiseOr:
            case NodeType::BitwiseXor:
            case NodeType::LeftShift:
            case NodeType::RightShift:
            // Arithmetic
            case NodeType::Add:
            case NodeType::Subtract:
            case NodeType::Multiply:
            case NodeType::Divide:
            case NodeType::Modulo:
                ReturnIfError(print_binary_operator((node)));
                break;
            // Unary operators
            case NodeType::LogicalNot:
            case NodeType::BitwiseNot:
            case NodeType::UnaryMinus:
                ReturnIfError(print_unary_operator((node)));
                break;
            //
            // Functions
            //
            case NodeType::Constructor:
                sb << '(' << node->_return_type << ')' << '(';
                for(size_t i = 0; i < node->_count; i++)
                {
                    if(i != 0)
                    {
                        sb << ',';
                    }
                    ReturnIfError(print_code(node->_children[i]));
                }
                sb << ')';
                break;
            case NodeType::Cast:
                SICKL_ASSERT(node->_count == 1);
                sb << '(' << node->_return_type << ')' << '(';
                ReturnIfError(print_code(node->_children[0]));
                sb << ')';
                break;
            case NodeType::BuiltinFunction:
                SICKL_ASSERT(node->_count >= 1);
                ReturnIfError(print_builtin_function(node));
                break;
            case NodeType::CallUserFunction:
                SICKL_ASSERT(node->_count >= 1);
                sb << node->_sid << '(';
                for(size_t k = 0; k < node->_count; k++)
                {
                    if(k > 0)
                    {
                        sb <<',';
                    }
                    ASTNode* currentNode = node->_children[k];
                    if(currentNode->_return_type & ReturnType::Buffer1D)
                    {
                        symbol_id_t sid = currentNode->_sid;
                        sb << sid << "_length,";
                        sb << sid;
                    }
                    else if(currentNode->_return_type & ReturnType::Buffer2D)
                    {
                        symbol_id_t sid = currentNode->_sid;
                        sb << sid << "_width,";
                        sb << sid << "_height,";
                        sb << sid;
                    }
                    else
                    {
                        ReturnIfError(print_code(currentNode));
                    }
                }
                sb << ')';
                break;
            //
            // Samplers
            //
            case NodeType::Sample1D:
                SICKL_ASSERT(node->_count == 2);
                sb << node->_children[0]->_sid << '[';
                ReturnIfError(print_code(node->_children[1]));
                sb << ']';
                break;
            case NodeType::Sample2D:
                SICKL_ASSERT(node->_count == 3);
                SICKL_ASSERT(node->_children[1]->_return_type == ReturnType::Int ||
                             node->_children[1]->_return_type == ReturnType::UInt);
                SICKL_ASSERT(node->_children[2]->_return_type == ReturnType::Int ||
                             node->_children[2]->_return_type == ReturnType::UInt);
                {
                    const symbol_id_t buffer_sid = node->_children[0]->_sid;
                    sb << buffer_sid << '[';
                    ReturnIfError(print_code(node->_children[2]));
                    sb << " * " << buffer_sid << "_width + ";
                    ReturnIfError(print_code(node->_children[1]));
                    sb << ']';
                }   
                break;
            default:
                break;
                SICKL_ASSERT(false);
            }

            return SICKL_SUCCESS;
        }

        sickl_int print_function(const ASTNode* in_func_root)
        {
            indent = 0;
        
            SICKL_ASSERT(in_func_root->_count == 2);
            ASTNode* func_params = in_func_root->_children[0];
            ASTNode* func_body = in_func_root->_children[1];
            
            symbol_id_t func_sid = in_func_root->_sid;
            declared_vars.insert(func_sid);
            
            if(in_func_root->_node_type == NodeType::Main)
            {
                sb << "__kernel ";
            }
            
            sb << in_func_root->_return_type << " " << func_sid  << "(";
            
            for(size_t k = 0; k < func_params->_count; k++)
            {
                ASTNode* current_param = func_params->_children[k];
                ReturnType_t type = current_param->_return_type;
                symbol_id_t sid = current_param->_sid;
                
                sb << newline;

                if(type & ReturnType::Buffer1D)
                {
                    sb << " ";
                    sb << "const " << ReturnType::UInt << ' ' << sid << "_length," << newline;
                }
                else if(type & ReturnType::Buffer2D)
                {
                    sb << " ";
                    sb << "const " << ReturnType::UInt << ' ' << sid << "_width," << newline;
                    sb << " ";
                    sb << "const " << ReturnType::UInt << ' ' << sid << "_height," << newline;
                }

                sb << " ";
                if(type & (ReturnType::Buffer1D | ReturnType::Buffer2D))
                {
                    sb << "__global ";
                }
                sb << type << ' ' << sid;
                if(k < (func_params->_count - 1))
                {
                    sb << ',';
                }

                declared_vars.insert(sid);
            }
            
            sb << ')' << newline;
            sb << '{' << newline;
            
            ReturnIfError(print_code(func_body));
            
            sb << '}' << newline;
            
            return SICKL_SUCCESS;
        }

        sickl_int print_kernel_source(const ASTNode* in_root)
        {
            for(size_t k = 0; k < in_root->_count; k++)
            {
                ASTNode* currentNode = in_root->_children[k];
                SICKL_ASSERT(currentNode->_node_type == NodeType::Function ||
                             currentNode->_node_type == NodeType::Main);
                
                print_function(currentNode);
            }
            return SICKL_SUCCESS;
        }

        sickl_int Reset()
        {
            ReturnIfError(sb.Clear());
            declared_vars.clear();

            return SICKL_SUCCESS;
        }
    }

    sickl_int Compiler::Build(Source& in_source, Program& out_program)
    {
        in_source.Parse();

        const ASTNode* root = in_source.GetRoot();
        root->Print();
        
        ReturnIfError(Internal::Reset());

        ReturnIfError(Internal::print_kernel_source(in_source.GetRoot()));

        printf("\n%s\n", (const char*)Internal::sb);
        fflush(stdout);

        Program result;
        {
            // create program
            const char* source = Internal::sb;
            cl_int err = CL_SUCCESS;
            cl_program program = clCreateProgramWithSource(Runtime::_context, 1, &source, nullptr, &err);
            SICKL_ASSERT(err == CL_SUCCESS);

            // build it
            err = clBuildProgram(program, 1, &Runtime::_device, nullptr, nullptr, nullptr);
            if(err == CL_BUILD_PROGRAM_FAILURE)
            {
                size_t log_size;
                clGetProgramBuildInfo(program, Runtime::_device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);

                // Allocate memory for the log
                char *log = new char[log_size];

                // Get the log
                clGetProgramBuildInfo(program, Runtime::_device, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);

                // Print the log
                printf("%s\n", log);

                delete[] log;

                SICKL_ASSERT(!"Kernel Build Error");
            }
            SICKL_ASSERT(err == CL_SUCCESS);

            // get entry point
            
            // find the main node
            const ASTNode* main = nullptr;
            for(size_t k = 0; k < root->_count; k++)
            {
                const ASTNode* currentChild = root->_children[k];
                if(currentChild->_node_type == NodeType::Main)
                {
                    main = currentChild;
                }
            }
            
            SICKL_ASSERT(main != nullptr);
            
            Internal::StringBuffer main_sb;
            main_sb << main->_sid;
            
            cl_kernel kernel = clCreateKernel(program, main_sb, &err);
            SICKL_ASSERT(err == CL_SUCCESS);

            result._program = program;
            result._kernel = kernel;

            ASTNode* params= main->_children[0];
            SICKL_ASSERT(params->_node_type == NodeType::Parameters);

            const size_t type_count = params->_count;
            ReturnType_t* types = new ReturnType_t[type_count];
            for(size_t k = 0; k < type_count; k++)
            {
                types[k] = params->_children[k]->_return_type;
            }

            result._types = types;

            result._type_count =  type_count;
        }
        out_program = result;

        return SICKL_SUCCESS;
    }
}

