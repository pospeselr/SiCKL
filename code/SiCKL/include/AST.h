#pragma once

/*
 * Abstract Syntax Tree Node.  A AST is built at runtime on call of Source::Parse();
 */

namespace SiCKL
{
    struct ASTNode
    {
        ASTNode();
        ASTNode(const ASTNode&);
        ASTNode(NodeType_t, ReturnType::Type);
        ASTNode(NodeType_t, ReturnType::Type, symbol_id_t);
        template<typename T>
        ASTNode(NodeType_t node_type, ReturnType::Type return_type, const T* data_pointer)
            : _node_type(node_type)
            , _count(0)
            , _capacity(1)
            , _return_type(return_type)
            , _sid(invalid_symbol)
            , _literal({0,0})
        {
            SICKL_ASSERT(node_type == NodeType::Literal);
            // copy in raw data
            _literal.data = malloc(sizeof(T));
            _literal.size = sizeof(T);
            ::memcpy(_literal.data, data_pointer, _literal.size);


            _children = new ASTNode*[_capacity];
            _children[0] = nullptr;
        }
        ~ASTNode();
        void add_child(ASTNode*);

        NodeType_t _node_type;
        size_t _count;
        size_t _capacity;
        ASTNode** _children;

        ReturnType::Type _return_type;


        symbol_id_t _sid;
        struct
        {
            void* data;
            size_t size;
        } _literal;

        void Print() const;
        void PrintNode() const;
        void PrintDot() const;
    private:
        static void Print(const ASTNode*, uint32_t indent);
        static void PrintDot(const ASTNode*, uint32_t& id);
    };

    template<bool B, class T = void>
    struct enable_if { };

    template<class T>
    struct enable_if<true, T> { typedef T type; };

    // used to determine if a type is an OpenCL primitive (ie, a cl_*)
    template<typename T>
    struct is_opencl_primitive
    {
        const static bool value = false;
    };

    #define IS_OPENCL_PRIMITIVE(T) template<> struct is_opencl_primitive<T> {const static bool value = true;};
    IS_OPENCL_PRIMITIVE(bool)
    IS_OPENCL_PRIMITIVE(cl_int)
    IS_OPENCL_PRIMITIVE(cl_uint)
    IS_OPENCL_PRIMITIVE(cl_float)
    #undef IS_OPENCL_PRIMITIVE

    // used to determine if a type is derived from the SiCKL::Data struct
    template<typename T>
    struct is_data_derived
    {
        const static bool value = false;
    };

    #define IS_DATA_DERIVED(T) struct T; template<> struct is_data_derived<SiCKL::T> {const static bool value = true;};
    IS_DATA_DERIVED(Bool)
    IS_DATA_DERIVED(Int)
    IS_DATA_DERIVED(Int2)
    IS_DATA_DERIVED(Int3)
    IS_DATA_DERIVED(Int4)
    IS_DATA_DERIVED(UInt)
    IS_DATA_DERIVED(UInt2)
    IS_DATA_DERIVED(UInt3)
    IS_DATA_DERIVED(UInt4)
    IS_DATA_DERIVED(Float)
    IS_DATA_DERIVED(Float2)
    IS_DATA_DERIVED(Float3)
    IS_DATA_DERIVED(Float4)
    #undef IS_DATA_DERIVED

    template<typename T>
    struct return_type
    {
        // base case for kernel buffer objects
        const static ReturnType_t type = T::_return_type;
    };

    #define PRIMITIVE_TO_RETURNTYPE(P, RT) template<> struct return_type<P> {const static ReturnType_t type = RT;};
    PRIMITIVE_TO_RETURNTYPE(bool, ReturnType::Bool)
    PRIMITIVE_TO_RETURNTYPE(cl_int, ReturnType::Int)
    PRIMITIVE_TO_RETURNTYPE(cl_uint, ReturnType::UInt)
    PRIMITIVE_TO_RETURNTYPE(cl_float, ReturnType::Float)
    #undef PRIMITIVE_TO_RETURNTYPE

    /// Data Node Creation

    template<typename TYPE>
    static ASTNode* create_literal_node(const TYPE& val)
    {
        static_assert(is_opencl_primitive<TYPE>::value == true, "Literal nodes must contain primitive POD type");
        return new ASTNode(NodeType::Literal, return_type<TYPE>::type, &val);
    }

    template<typename TYPE>
    static ASTNode* create_data_node(const TYPE& val)
    {
        static_assert(is_data_derived<TYPE>::value == true, "Data nodes must be a SiCKL::Data derived type");
        if(val._id >= 0)
        {
            // a symbol
            return new ASTNode(NodeType::Var, return_type<TYPE>::type, val._id);
        }
        else
        {
            // make sure the temp data is initialized
            SICKL_ASSERT(val._node != nullptr);
            return new ASTNode(*val._node);
        }
        return nullptr;
    }

    // different create_value_node based on the template parameter

    // create value node for primitive type
    template<typename TYPE>
    static typename enable_if<is_opencl_primitive<TYPE>::value, ASTNode*>::type
    create_value_node(const TYPE& val)
    {
        //make a literal node
        return create_literal_node<TYPE>(val);
    }

    // create value node for reference type
    template<typename TYPE>
    static typename enable_if<is_data_derived<TYPE>::value, ASTNode*>::type
    create_value_node(const TYPE& val)
    {
        // if it's not a primitive, it must be a Data
        return create_data_node<TYPE>(val);
    }

    /// Unary and Binary operator methods

    template<typename BASE>
    static void unary_operator(struct ASTNode* root, const BASE& in_base)
    {
        static_assert(is_data_derived<BASE>::value == true, "Unary operator should only be applied to SiCKL::Data derived types");

        ASTNode* child = create_data_node(in_base);
        root->add_child(child);
    }

    template<typename BASE, typename FRIEND>
    static void binary_operator(ASTNode* root, const BASE& in_base, const FRIEND& in_friend)
    {
        ASTNode* left = create_value_node(in_base);
        ASTNode* right = create_value_node(in_friend);

        root->add_child(left);
        root->add_child(right);
    }
}
