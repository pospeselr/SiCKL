#include "SiCKL.h"
#include "SiCKLUndef.h"

namespace SiCKL
{
    const char* NodeTypes[] =
    {
        "invalid",
        "program",
        "main",
        "function",
        "parameter_block",
        "body_block",
        "return",

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
        "builtin_function",
        "call_user_function",

        "sample1d",
        "sample2d",
        "member",
    };

    static_assert(count_of(NodeTypes) == (NodeType::Max + 1), "Mismatch between NodeTypes string and enum");

#define RETURN_TYPE_SWITCH(F, B)	\
    switch(type)\
    {\
    case DataType::Bool:\
        return F "bool" B;\
    case DataType::Int:\
        return F "int" B;\
    case DataType::UInt:\
        return F "uint" B;\
    case DataType::Float:\
        return F "float" B;\
    case DataType::Int2:\
        return F "int2" B;\
    case DataType::UInt2:\
        return F "uint2" B;\
    case DataType::Float2:\
        return F "float2" B;\
    case DataType::Int3:\
        return F "int3" B;\
    case DataType::UInt3:\
        return F "uint3" B;\
    case DataType::Float3:\
        return F "float3" B;\
    case DataType::Int4:\
        return F "int4" B;\
    case DataType::UInt4:\
        return F "uint4" B;\
    case DataType::Float4:\
        return F "float4" B;\
    default:\
        SICKL_ASSERT(false);\
    }\

    const char* GetDataType(DataType::Type type)
    {
        if(type == DataType::Invalid)
        {
            return "invalid";
        }
        else if(type == DataType::Void)
        {
            return "void";
        }

        if(type & DataType::Buffer1D)
        {
            type = (DataType::Type)(type ^ DataType::Buffer1D);
            SICKL_ASSERT(type != DataType::Void);
            RETURN_TYPE_SWITCH("buffer1d<",">")
        }
        else if(type & DataType::Buffer2D)
        {
            type = (DataType::Type)(type ^ DataType::Buffer2D);
            SICKL_ASSERT(type != DataType::Void);
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
        , _data_type(DataType::Invalid)
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
        _data_type = in_node._data_type;

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

    ASTNode::ASTNode(NodeType_t node_type, DataType::Type data_type)
        : _node_type(node_type)
        , _count(0)
        , _capacity(1)
        , _data_type(data_type)
        , _sid(invalid_symbol)
        , _literal({0,0})
    {
        _children = new ASTNode*[_capacity];
        _children[0] = nullptr;
    }

    ASTNode::ASTNode(NodeType_t node_type, DataType::Type data_type, symbol_id_t sid)
        : _node_type(node_type)
        , _count(0)
        , _capacity(1)
        , _data_type(data_type)
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
        printf("%s -> %s", NodeTypes[_node_type + 1], GetDataType(_data_type));
        switch(_node_type)
        {
        case NodeType::Var:
        case NodeType::ConstVar:
        case NodeType::OutVar:
        case NodeType::Function:
        case NodeType::Main:
            printf(", symbol = 0x%x", static_cast<uint32_t>(_sid));
            break;
        case NodeType::Literal:
            switch(_data_type)
            {
            case DataType::Bool:
                printf(", val = %s", *(bool*)_literal.data ? "true" : "false");
                break;
            case DataType::Int:
                printf(", val = %i", *(cl_int*)_literal.data);
                break;
            case DataType::UInt:
                printf(", val = %u", *(cl_uint*)_literal.data);
                break;
            case DataType::Float:
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


    /// node creation
    ASTNode* create_return_node(DataType_t type)
    {
        return new ASTNode(NodeType::Return, type);
    }
}
