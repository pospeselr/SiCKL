#pragma once

/*
 * Definition of our SiCKL::Source struct used to define our kernel programs
 * Also contains all the various macro definitions required to make the syntax pretty
 */

namespace SiCKL
{
    struct ASTNode;

    // forward declare all of our Kernel types
    struct Bool;
    struct Int;
    struct Int2;
    struct Int3;
    struct Int4;
    struct UInt;
    struct UInt2;
    struct UInt3;
    struct UInt4;
    struct Float;
    struct Float2;
    struct Float3;
    struct Float4;

    template<typename T> struct Buffer1D;
    template<typename T> struct Buffer2D;

    template<typename T> struct Function;

    class Source
    {
    public:
        Source();
        ~Source();

        const ASTNode* GetRoot() const {return _my_ast;}
        uint32_t GetSymbolCount() const {return _symbol_count;}

    private:
        static ASTNode* _root;
        static ASTNode* _current_block;
        static ASTNode* _current_function;
        static std::vector<ASTNode*> _block_stack;
        static symbol_id_t _next_symbol;

        // the AST for this program's Source
        ASTNode* _my_ast;
        uint32_t _symbol_count;
        
        // needed so Compiler can access the 'Parse' method
        friend class Compiler;
    
        // friend all these types
        friend struct Bool;
        friend struct Int;
        friend struct Int2;
        friend struct Int3;
        friend struct Int4;
        friend struct UInt;
        friend struct UInt2;
        friend struct UInt3;
        friend struct UInt4;
        friend struct Float;
        friend struct Float2;
        friend struct Float3;
        friend struct Float4;

        template<typename BASE, typename PARENT>
        friend struct Member;

        template<typename T>
        friend struct Function;
        template<typename T>
        friend struct function_parameter_generator;

        template<typename T, typename... Args>
        friend struct construct;
        friend struct function_body_construct;
        friend struct if_construct;
        friend struct elseif_construct;
        friend struct else_construct;
        friend struct while_construct;
        friend struct forinrange_construct;

        friend struct return_statement;
    protected:
    
        void initialize();
        void finalize(Function<void>& main);
    
        static symbol_id_t next_symbol();
        static symbol_id_t current_symbol();

        static bool start_block(ASTNode* block);
        static void add_to_current_block(ASTNode* node);

        static bool end_block();

        // Parse method defined in the KernelMain macro
        virtual void Parse() = 0;
    };

    // defines various struct types used for creating new programs



    // all the SiCKL::Source::TYPES extend Data
    struct Data
    {
        Data()
        : _id(invalid_symbol)
        , _node(nullptr)
        { }
        
        Data(symbol_id_t in_id, struct ASTNode* in_node)
        : _id(in_id)
        , _node(in_node)
        { }

        ~Data()
        {
            delete _node;
        }

        mutable symbol_id_t _id;
        struct ASTNode* _node;
    };

    // LValues can be assigned to
    template<typename BASE>
    struct LValue : public BASE
    {
        LValue(ASTNode* in_node)
            : BASE(lvalue_symbol, in_node)
        {}
        using BASE::operator=;
    };
    
    // RValues don't have permanent storage in memory
    template<typename BASE>
    struct RValue : public BASE
    {
        RValue(ASTNode* in_node)
            : BASE(rvalue_symbol, in_node)
        {}
    
        // delete the assignment operator
        template<typename RIGHT>
        RValue& operator=(const RIGHT&) = delete;
    };
    
    // member variables are referred to by index
    template<typename BASE, typename PARENT>
    struct Member : public BASE
    {
        using BASE::operator=;
        Member(uintptr_t member_offset, member_id_t mid) : BASE(member_symbol, nullptr)
        {
            // get our owner
            PARENT* parent = (PARENT*)(((uint8_t*)this) - member_offset);
    
            // in the case where our parent did not get anything assigned to it, give it a valid symbol
            if(parent->_id == invalid_symbol)
            {
                parent->_id = Source::next_symbol();
            }
    
            // construct our ASTnode that represents us
            ASTNode* member = new ASTNode(NodeType::Member, data_type<BASE>::type);
            binary_operator(member, *parent, mid);
    
            this->_node = member;
        }
    };
    
    template<typename T>
    struct Function
    {
        Function(std::nullptr_t)
        {
            func_root = new ASTNode(NodeType::Function, data_type<T>::type, Source::next_symbol());
            Source::start_block(func_root);
            Source::_current_function = func_root;
        }
        
        template<typename...Args>
        const RValue<T> operator()(const Args&... args)
        {
            ASTNode* funcCall = new ASTNode(NodeType::CallUserFunction, data_type<T>::type, func_root->_sid);
        
            BuildCallNode(funcCall, 0, args...);
        
            return RValue<T>(funcCall);
        }
        
        friend class Source;
    private:
        
        template<typename Arg, typename...Args>
        void BuildCallNode(ASTNode* funcCall, size_t index, const Arg& arg, const Args&... args) const
        {
            const ASTNode* params = func_root->_children[0];
            // verify we don't have too many params
            SICKL_ASSERT(index < params->_count);
            const ASTNode* currentParam = params->_children[index];
            // verify the passed in type is correct
            SICKL_ASSERT(currentParam->_data_type == data_type<Arg>::type);
            
            funcCall->add_child(create_value_node(arg));
            
            BuildCallNode(funcCall, ++index, args...);
        }
    
        void BuildCallNode(ASTNode* funcCall, size_t index) const
        {
            const ASTNode* params = func_root->_children[0];
            // ensure we have enough params
            SICKL_ASSERT(index == params->_count);
            
            (void)funcCall;
        }
    
        ASTNode* func_root;
    };    
}

// foreach macro for function definitions

#define EXPAND(x) x
#define FOR_EACH_1(WORK, x1) WORK(x1, 1)
#define FOR_EACH_2(WORK, x1, x2) WORK(x1, 1) WORK(x2, 2)
#define FOR_EACH_3(WORK, x1, x2, x3) WORK(x1, 1) WORK(x2, 2) WORK(x3, 3)
#define FOR_EACH_4(WORK, x1, x2, x3, x4) WORK(x1, 1) WORK(x2, 2) WORK(x3, 3) WORK(x4, 4)
#define FOR_EACH_5(WORK, x1, x2, x3, x4, x5) WORK(x1, 1) WORK(x2, 2) WORK(x3, 3) WORK(x4, 4) WORK(x5, 5)
#define FOR_EACH_6(WORK, x1, x2, x3, x4, x5, x6) WORK(x1, 1) WORK(x2, 2) WORK(x3, 3) WORK(x4, 4) WORK(x5, 5) WORK(x6, 6)
#define FOR_EACH_7(WORK, x1, x2, x3, x4, x5, x6, x7) WORK(x1, 1) WORK(x2, 2) WORK(x3, 3) WORK(x4, 4) WORK(x5, 5) WORK(x6, 6) WORK(x7, 7)
#define FOR_EACH_8(WORK, x1, x2, x3, x4, x5, x6, x7, x8) WORK(x1, 1) WORK(x2, 2) WORK(x3, 3) WORK(x4, 4) WORK(x5, 5) WORK(x6, 6) WORK(x7, 7) WORK(x8, 8)

#define FOR_EACH_NARG(...) FOR_EACH_NARG_(__VA_ARGS__, FOR_EACH_RSEQ_N())
#define FOR_EACH_NARG_(...) EXPAND(FOR_EACH_ARG_N(__VA_ARGS__))
#define FOR_EACH_ARG_N(_1, _2, _3, _4, _5, _6, _7, _8, N, ...) N
#define FOR_EACH_RSEQ_N() 8, 7, 6, 5, 4, 3, 2, 1, 0
#define CONCATENATE(x,y) x##y
#define FOR_EACH_(N, what, ...) EXPAND(CONCATENATE(FOR_EACH_, N)(what, __VA_ARGS__))
#define FOR_EACH(what, ...) FOR_EACH_(FOR_EACH_NARG(__VA_ARGS__), what, __VA_ARGS__)

// macros for defining new constructs (ie something that happens on scope enter, and scope exit)
#define MAKE_CONSTRUCT_0(CON, NAME) for(CON NAME; !NAME.finished(); NAME.finish())
#define MAKE_CONSTRUCT_N(CON, NAME, ...) for(CON NAME(__VA_ARGS__); !NAME.finished(); NAME.finish())

#define BEGIN_SOURCE void Parse() { initialize();
#define END_SOURCE finalize(main); }

// macros for making unique name for constructs
#define __GLUE2(A,B) A ## B
#define __UNIQUE_NAME2(A,B) __GLUE2(A,B)

#define __GLUE3(A,B,C) A ## B ## C
#define __UNIQUE_NAME3(A,B,C) __GLUE3(A,B,C)

#define __GLUE4(A,B,C,D) A ## B ## C ## D
#define __UNIQUE_NAME4(A,B,C,D) __GLUE4(A,B,C,D)

#define If(b)\
    MAKE_CONSTRUCT_N(if_construct, __UNIQUE_NAME2(__if_, __LINE__), b)

#define ElseIf(b)\
    MAKE_CONSTRUCT_N(elseif_construct, __UNIQUE_NAME2(__elseif_, __LINE__), b)
    
#define Else\
    MAKE_CONSTRUCT_0(else_construct, __UNIQUE_NAME2(__else_, __LINE__))
    
#define While(b)\
    MAKE_CONSTRUCT_N(while_construct, __UNIQUE_NAME2(__while_, __LINE__), b)

#define ForInRange(I, START, STOP)\
    MAKE_CONSTRUCT_N(forinrange_construct, I, START, STOP)

#define MakeFunction(...)\
MAKE_ALL_PARAM_TYPES(__VA_ARGS__)\
MAKE_ALL_PARAM_GENERATORS(__VA_ARGS__)\
MAKE_ALL_FUNC_PARAMS(__VA_ARGS__)

#define MAKE_PARAM_TYPE(DECL, N) DECL, param##N; typedef decltype(param##N) type##N;
#define MAKE_ALL_PARAM_TYPES(...)\
nullptr;\
struct __UNIQUE_NAME2(param_types, __LINE__)\
{\
    FOR_EACH(MAKE_PARAM_TYPE, __VA_ARGS__)\
};
#define MAKE_PARAM_GENERATOR(X, N) function_parameter<__UNIQUE_NAME2(param_types, __LINE__)::__GLUE2(type,N)> __UNIQUE_NAME4(make_param, __LINE__, _, N);
#define MAKE_ALL_PARAM_GENERATORS(...) FOR_EACH(MAKE_PARAM_GENERATOR, __VA_ARGS__)
#define MAKE_FUNC_PARAM(X, N) for(X : __UNIQUE_NAME4(make_param, __LINE__, _, N) )
#define MAKE_ALL_FUNC_PARAMS(...)\
if(Source::start_block(new ASTNode(NodeType::Parameters, DataType::Void)))\
FOR_EACH(MAKE_FUNC_PARAM, __VA_ARGS__)\
if(Source::end_block())\
MAKE_CONSTRUCT_0(function_body_construct, __function_body_)

#define Return return_statement __UNIQUE_NAME2(ret_, __LINE__); __UNIQUE_NAME2(ret_, __LINE__) =
