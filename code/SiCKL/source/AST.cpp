#include "SiCKL.h"

namespace SiCKL
{
    const char* NodeTypes[] =
    {
        "invalid",
        "program",
        "main_block",
        "parameter_block",

        "if",
        "elseif",
        "else",
        "while",
        "forinrange",

        "out_var",
        "const_var",
        "var",
        "literal",

        ":=",

        "==",
        "!=",
        ">",
        ">=",
        "<",
        "<=",

        "and",
        "or",
        "not",

        "&",
        "|",
        "^",
        "~",

        "<<",
        ">>",

        "-",
        "+",
        "-",
        "*",
        "/",
        "%",

        "constructor",
        "cast",
        "function",

        "sample1d",
        "sample2d",
        "member",
    };

    static_assert(count_of(NodeTypes) == (NodeType::Max + 1), "Mismatch between NodeTypes string and enum");

#define RETURN_TYPE_SWITCH(F, B)	\
    switch(type)\
    {\
    case ReturnType::Bool:\
        return F "bool" B;\
    case ReturnType::Int:\
        return F "int" B;\
    case ReturnType::UInt:\
        return F "uint" B;\
    case ReturnType::Float:\
        return F "float" B;\
    case ReturnType::Int2:\
        return F "int2" B;\
    case ReturnType::UInt2:\
        return F "uint2" B;\
    case ReturnType::Float2:\
        return F "float2" B;\
    case ReturnType::Int3:\
        return F "int3" B;\
    case ReturnType::UInt3:\
        return F "uint3" B;\
    case ReturnType::Float3:\
        return F "float3" B;\
    case ReturnType::Int4:\
        return F "int4" B;\
    case ReturnType::UInt4:\
        return F "uint4" B;\
    case ReturnType::Float4:\
        return F "float4" B;\
    default:\
        SICKL_ASSERT(false);\
    }\

    const char* GetReturnType(ReturnType::Type type)
    {
        if(type == ReturnType::Invalid)
        {
            return "invalid";
        }
        else if(type == ReturnType::Void)
        {
            return "void";
        }

        if(type & ReturnType::Buffer1D)
        {
            type = (ReturnType::Type)(type ^ ReturnType::Buffer1D);
            SICKL_ASSERT(type != ReturnType::Void);
            RETURN_TYPE_SWITCH("buffer1d<",">")
        }
        else if(type & ReturnType::Buffer2D)
        {
            type = (ReturnType::Type)(type ^ ReturnType::Buffer2D);
            SICKL_ASSERT(type != ReturnType::Void);
            RETURN_TYPE_SWITCH("buffer2d<",">")
        }
        else
        {
            RETURN_TYPE_SWITCH("","")
        }
        SICKL_ASSERT(false);
        return "unknown";
    }

    /// AST related guff

    ASTNode::ASTNode()
        : _node_type(NodeType::Invalid)
        , _count(0)
        , _capacity(1)
        , _return_type(ReturnType::Invalid)
        , _sid(invalid_symbol)
        , _literal({0,0})
    {
        _children = new ASTNode*[_capacity];
        _children[0] = nullptr;
    }

    ASTNode::ASTNode(const ASTNode& in_node)
        : _sid(invalid_symbol)
        , _literal({0,0})
    {
        _node_type = in_node._node_type;
        _return_type = in_node._return_type;

        _sid = in_node._sid;
        if(_node_type == NodeType::Literal)
        {
            _literal.size = in_node._literal.size;
            _literal.data = malloc(_literal.size);
            memcpy(_literal.data, in_node._literal.data, _literal.size);
        }


        _count = in_node._count;
        if(_count == 0)
        {
            _capacity = 1;
            _children = new ASTNode*[1];
            _children[0] = nullptr;
        }
        else
        {
            _capacity = _count;
            _children = new ASTNode*[_capacity];
            for(uint32_t i = 0; i < _count; i++)
            {
                _children[i] = new ASTNode(*in_node._children[i]);
            }
        }
    }

    ASTNode::ASTNode(NodeType_t node_type, ReturnType::Type return_type)
        : _node_type(node_type)
        , _count(0)
        , _capacity(1)
        , _return_type(return_type)
        , _sid(invalid_symbol)
        , _literal({0,0})
    {
        _children = new ASTNode*[_capacity];
        _children[0] = nullptr;
    }

    ASTNode::ASTNode(NodeType_t node_type, ReturnType::Type return_type, symbol_id_t sid)
        : _node_type(node_type)
        , _count(0)
        , _capacity(1)
        , _return_type(return_type)
        , _sid(sid)
        , _literal({0,0})
    {
        _children = new ASTNode*[_capacity];
        _children[0] = nullptr;
    }

    ASTNode::~ASTNode()
    {
        if(_node_type == NodeType::Literal)
        {
            free(_literal.data);
        }

        for(uint32_t i = 0; i < _count; i++)
        {
            delete _children[i];
        }
        delete[] _children;
    }

    void ASTNode::add_child(ASTNode* node)
    {
        _children[_count++] = node;
        // increase space if necessary
        if(_count == _capacity)
        {
            // double capacity
            _capacity *= 2;
            ASTNode** temp = new ASTNode*[_capacity];
            // copy over children to new buffer
            for(size_t i = 0; i < _count; i++)
            {
                temp[i] = _children[i];
            }
            // set the rest to null
            for(size_t i = _count; i < _capacity; i++)
            {
                temp[i] = nullptr;
            }
            // delete old buffer
            delete[] _children;
            // and set new
            _children = temp;
        }
    }

    void ASTNode::Print() const
    {
        Print(this, 0);
    }

    void ASTNode::PrintNode() const
    {
        printf("%s -> %s", NodeTypes[_node_type + 1], GetReturnType(_return_type));
        switch(_node_type)
        {
        case NodeType::Var:
        case NodeType::ConstVar:
        case NodeType::OutVar:
            printf(", symbol = 0x%x", static_cast<uint32_t>(_sid));
            break;
        case NodeType::Literal:
            switch(_return_type)
            {
            case ReturnType::Bool:
                printf(", val = %s", *(bool*)_literal.data ? "true" : "false");
                break;
            case ReturnType::Int:
                printf(", val = %i", *(cl_int*)_literal.data);
                break;
            case ReturnType::UInt:
                printf(", val = %u", *(cl_uint*)_literal.data);
                break;
            case ReturnType::Float:
                printf(", val = %f", *(cl_float*)_literal.data);
                break;
            default:
                SICKL_ASSERT(false);
                break;
            }
            break;
        default:
            break;
        }
    }

    void ASTNode::Print(const ASTNode* in_node, uint32_t indent)
    {
        for(uint32_t i = 0; i < indent; i++)
        {
            printf(" ");
        }

        in_node->PrintNode();

        printf("\n");

        for(uint32_t j = 0; j < in_node->_count; j++)
        {
            Print(in_node->_children[j], indent + 1);
        }
    }

    void ASTNode::PrintDot() const
    {
        printf("digraph AST\n{\n");
        printf(" node [fontsize=12, shape=box];\n");
        printf(" rankdir=LR;\n");

        uint32_t id = 0;

        PrintDot(this, id);

        printf("}\n");
    }

    void ASTNode::PrintDot(const ASTNode* in_node, uint32_t& id)
    {
        printf(" node%i [label=\"", id);
        in_node->PrintNode();
        printf("\"];\n");

        const uint32_t my_id = id;

        for(uint32_t j = 0; j < in_node->_count; j++)
        {
            ++id;
            printf(" node%i -> node%i;\n", my_id, id);

            PrintDot(in_node->_children[j], id);
        }
    }

}
