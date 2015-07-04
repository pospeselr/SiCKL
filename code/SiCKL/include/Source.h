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

        template<typename T, typename... Args>
        friend struct construct;
        friend struct kernelmain_construct;
        friend struct if_construct;
        friend struct elseif_construct;
        friend struct else_construct;
        friend struct while_construct;
        friend struct forinrange_construct;
    protected:
    
        void initialize();
        void finalize();
    
        static symbol_id_t next_symbol();
        static symbol_id_t current_symbol();

        static void start_block(ASTNode* block);
        static void add_to_current_block(ASTNode* node);

        static void end_block();

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
            ASTNode* member = new ASTNode(NodeType::Member, return_type<BASE>::type);
            binary_operator(member, *parent, mid);
    
            this->_node = member;
        }
    };
}

// foreach macro for function definitions

#define EXPAND(x) x
#define FOR_EACH_1(what, x, ...) what(x)
#define FOR_EACH_2(what, x, ...)\
  what(x)\
  EXPAND(FOR_EACH_1(what,  __VA_ARGS__))
#define FOR_EACH_3(what, x, ...)\
  what(x)\
  EXPAND(FOR_EACH_2(what, __VA_ARGS__))
#define FOR_EACH_4(what, x, ...)\
  what(x)\
  EXPAND(FOR_EACH_3(what,  __VA_ARGS__))
#define FOR_EACH_5(what, x, ...)\
  what(x)\
  EXPAND(FOR_EACH_4(what,  __VA_ARGS__))
#define FOR_EACH_6(what, x, ...)\
  what(x)\
  EXPAND(FOR_EACH_5(what,  __VA_ARGS__))
#define FOR_EACH_7(what, x, ...)\
  what(x)\
  EXPAND(FOR_EACH_6(what,  __VA_ARGS__))
#define FOR_EACH_8(what, x, ...)\
  what(x)\
  EXPAND(FOR_EACH_7(what,  __VA_ARGS__))
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
#define END_SOURCE finalize(); }

#define BEGIN_PARAMS Source::start_block(new ASTNode(NodeType::Parameters, ReturnType::Void));
#define PARAM_DATA(VAR_DECLARE)\
    VAR_DECLARE(Source::next_symbol(), nullptr);\
    {\
        const symbol_id_t id = Source::current_symbol();\
        struct variable\
        {\
            VAR_DECLARE, UNUSED;\
            typedef decltype(UNUSED) type;\
        };\
        ASTNode* param = new ASTNode(NodeType::Var, return_type<variable::type>::type, id);\
        Source::add_to_current_block(param);\
    }
#define END_PARAMS Source::end_block();

// macros for making unique name for constructs
#define __glue(A,B) A ## B
#define __unique_name(A,B) __glue(A,B)

#define KernelMain(...)\
    BEGIN_PARAMS\
        FOR_EACH(PARAM_DATA, __VA_ARGS__)\
    END_PARAMS\
    MAKE_CONSTRUCT_0(kernelmain_construct, __unique_name(__kernelmain_, __LINE__))

#define If(b)\
    MAKE_CONSTRUCT_N(if_construct, __unique_name(__if_, __LINE__), b)

#define ElseIf(b)\
    MAKE_CONSTRUCT_N(elseif_construct, __unique_name(__elseif_, __LINE__), b)
    
#define Else\
    MAKE_CONSTRUCT_0(else_construct, __unique_name(__else_, __LINE__))
    
#define While(b)\
    MAKE_CONSTRUCT_N(while_construct, __unique_name(__while_, __LINE__), b)

#define ForInRange(I, START, STOP)\
    MAKE_CONSTRUCT_N(forinrange_construct, I, START, STOP)

