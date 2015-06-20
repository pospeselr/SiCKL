/*
 * This header is used to make Types.h and Functions.h be read as as forward declerations
 */

/// Types

#define START_TYPE(X)\
struct X : public Data\
{\
    X() : Data() {}\
    X(symbol_id_t in_id, ASTNode* in_node) : Data(in_id, in_node) {}\
    static const ReturnType::Type _return_type = ReturnType::X;

#define START_BUFFER_TYPE(X)\
template<typename TYPE>\
struct X : public Data\
{\
    X() : Data() {}\
    X(symbol_id_t in_id, ASTNode* in_node) : Data(in_id, in_node) {}\
    static const ReturnType::Type _return_type = (ReturnType::Type)(TYPE::_return_type | ReturnType::X);

#define END_TYPE };

/// Constructors
    
#define CONSTRUCTOR_1(TYPE, A)\
    TYPE(const A&);\
    TYPE& operator=(const A&);

#define CONSTRUCTOR_2(TYPE, A, B)\
    TYPE(const A&, const B&);

#define CONSTRUCTOR_3(TYPE, A, B, C)\
    TYPE(const A&, const B&, const C&);

#define CONSTRUCTOR_4(TYPE, A, B, C, D)\
    TYPE(const A&, const B&, const C&, const D&);

/// Operators

#define BINARY_OP(RETURN, L, R, NT, OP)\
    friend const RValue<RETURN> operator OP(const L&, const R&);

// not increment or decrement though
#define UNARY_OP(RETURN, L, NT, OP)\
    friend const RValue<RETURN> operator OP(const L&);

#define CAST_OP(FROM, TO)\
    operator const TO() const;

/// Members

#define START_MEMBERS(X)
#define MEMBER_VAR(PARENT, TYPE, NAME, ID)\
struct Member_##NAME : public Member<TYPE, PARENT>\
{\
    using Member<TYPE, PARENT>::operator=;\
    Member_##NAME() : Member<TYPE, PARENT>(_member_offset, _mid) { }\
private:\
    const static uintptr_t _member_offset;\
    const static member_id_t _mid;\
} NAME;
#define END_MEMBERS

#define FUNC_0(RETURN, NAME)\
const RValue<RETURN> NAME();
#define FUNC_1(RETURN, NAME, A)\
const RValue<RETURN> NAME(const A&);
#define FUNC_2(RETURN, NAME, A, B)\
const RValue<RETURN> NAME(const A&, const B&);
#define FUNC_3(RETURN, NAME, A, B, C)\
const RValue<RETURN> NAME(const A&, const B&, const C&);
#define FUNC_4(RETURN, NAME, A, B, C, D)\
const RValue<RETURN> NAME(const A&, const B&, const C&, const D&);
