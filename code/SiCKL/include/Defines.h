/*
 * This header is used to make Types.h and Functions.h be read as implementations. 
 * It is included in Types.cpp
 */
 
/// Undef everything
#undef START_TYPE
#undef END_TYPE
#undef CONSTRUCTOR_1
#undef CONSTRUCTOR_2
#undef CONSTRUCTOR_3
#undef CONSTRUCTOR_4
#undef BINARY_OP
#undef UNARY_OP
#undef CAST_OP
#undef START_MEMBERS
#undef MEMBER_VAR
#undef END_MEMBERS
#undef FUNC_0
#undef FUNC_1
#undef FUNC_2
#undef FUNC_3
#undef FUNC_4

/// Types

#define START_TYPE(X)
#define END_TYPE

/// Constructors

#define CONSTRUCTOR_1(TYPE, A)\
TYPE::TYPE(const A& a) : Data()\
{\
    ASTNode* node = create_value_node(a);\
    *this = TYPE(temp_symbol, node);\
}\
TYPE& TYPE::operator=(const A& a)\
{\
    if(this->_id == invalid_symbol)\
    {\
        this->_id = Source::next_symbol();\
    }\
    ASTNode* assign = new ASTNode(NodeType::Assignment, return_type<TYPE>::type);\
    binary_operator(assign, *this, a);\
    Source::add_to_current_block(assign);\
    return *this;\
}

#define CONSTRUCTOR_2(TYPE, A, B)\
TYPE::TYPE(const A& a, const B& b)\
{\
    ASTNode* node = new ASTNode(NodeType::Constructor, return_type<TYPE>::type);\
    node->add_child(create_value_node(a));\
    node->add_child(create_value_node(b));\
    *this = TYPE(temp_symbol, node);\
}

#define CONSTRUCTOR_3(TYPE, A, B, C)\
TYPE::TYPE(const A& a, const B& b, const C& c)\
{\
    ASTNode* node = new ASTNode(NodeType::Constructor, return_type<TYPE>::type);\
    node->add_child(create_value_node(a));\
    node->add_child(create_value_node(b));\
    node->add_child(create_value_node(c));\
    *this = TYPE(temp_symbol, node);\
}

#define CONSTRUCTOR_4(TYPE, A, B, C, D)\
TYPE::TYPE(const A& a, const B& b, const C& c, const D& d)\
{\
    ASTNode* node = new ASTNode(NodeType::Constructor, return_type<TYPE>::type);\
    node->add_child(create_value_node(a));\
    node->add_child(create_value_node(b));\
    node->add_child(create_value_node(c));\
    node->add_child(create_value_node(d));\
    *this = TYPE(temp_symbol, node);\
}

/// Operators

#define BINARY_OP(RET, L, R, NT, OP)\
const RValue<RET> operator OP(const L& l, const R& r)\
{\
    ASTNode* node = new ASTNode(NodeType::NT, return_type<RET>::type);\
    binary_operator(node, l, r);\
    return RValue<RET>(node);\
}\

#define UNARY_OP(RET, L, NT, OP)\
const RValue<RET> operator OP(const L& l)\
{\
    ASTNode* node = new ASTNode(NodeType::NT, return_type<RET>::type);\
    unary_operator(node, l);\
    return RValue<RET>(node);\
}

#define CAST_OP(FROM, TO)\
FROM::operator const TO() const\
{\
    ASTNode* node = new ASTNode(NodeType::Cast, return_type<TO>::type);\
    node->add_child(create_value_node(*this));\
    return TO(temp_symbol, node);\
}

/// Members

#define START_MEMBERS(X)
#define MEMBER_VAR(PARENT, TYPE, NAME, ID)\
const uintptr_t PARENT::Member_##NAME::_member_offset = offsetof(PARENT, NAME);\
const SiCKL::member_id_t PARENT::Member_##NAME::_mid = ID;
#define END_MEMBERS

/// Functions

#define FUNC_HEADER(RETURN, NAME)\
ASTNode* node = new ASTNode(NodeType::BuiltinFunction, return_type<RETURN>::type);\
int32_t id = BuiltinFunction::NAME;\
ASTNode* ident = new ASTNode(NodeType::Literal, ReturnType::Int, &id);\
node->add_child(ident);

#define FUNC_INPUT(VAL)\
node->add_child(create_value_node(VAL));

#define FUNC_FOOTER(RETURN)\
return RValue<RETURN>(node);

#define FUNC_0(RETURN, NAME)\
const RValue<RETURN> NAME()\
{\
    FUNC_HEADER(RETURN, NAME)\
    FUNC_FOOTER(RETURN)\
}

#define FUNC_1(RETURN, NAME, A)\
const RValue<RETURN> NAME(const A& a)\
{\
    FUNC_HEADER(RETURN, NAME)\
    FUNC_INPUT(a)\
    FUNC_FOOTER(RETURN)\
}

#define FUNC_2(RETURN, NAME, A, B)\
const RValue<RETURN> NAME(const A& a, const B& b)\
{\
    FUNC_HEADER(RETURN, NAME)\
    FUNC_INPUT(a)\
    FUNC_INPUT(b)\
    FUNC_FOOTER(RETURN)\
}

#define FUNC_3(RETURN, NAME, A, B, C)\
const RValue<RETURN> NAME(const A& a, const B& b, const C& c)\
{\
    FUNC_HEADER(RETURN, NAME)\
    FUNC_INPUT(a)\
    FUNC_INPUT(b)\
    FUNC_INPUT(c)\
    FUNC_FOOTER(RETURN)\
}

#define FUNC_4(RETURN, NAME, A, B, C, D)\
const RValue<RETURN> NAME(const A& a, const B& b, const C& c, const D& d)\
{\
    FUNC_HEADER(RETURN, NAME)\
    FUNC_INPUT(a)\
    FUNC_INPUT(b)\
    FUNC_INPUT(c)\
    FUNC_INPUT(d)\
    FUNC_FOOTER(RETURN)\
}
