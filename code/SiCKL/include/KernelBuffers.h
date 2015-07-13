#pragma once

/*
 * Definitions for the Buffer1D and Buffer2D device-side classes
 * Included
 */

namespace SiCKL
{
    
    template<typename T>
    struct is_valid_buffer_type
    {
        const static bool value = is_data_derived<T>::value;
    };
    
    // exclude Bool from being a valid buffer type
    template<>
    struct is_valid_buffer_type<Bool>
    {
        const static bool value = false;
    };
    
    START_BUFFER_TYPE(Buffer1D)
        /// Prevent instantions of Buffer2Ds with invalid TYPE
        static_assert(is_data_derived<TYPE>::value, "Invalid type used in Buffer1D");
        /// sampling operators
        const RValue<TYPE> operator()(uint32_t A) const
        {
            ASTNode* sample  = new ASTNode(NodeType::Sample1D, return_type<TYPE>::type);
            sample->add_child(new ASTNode(NodeType::ConstVar, return_type<Buffer1D<TYPE>>::type, _id));
            sample->add_child(create_literal_node(A));
            return RValue<TYPE>(sample);
        }
    
        const RValue<TYPE> operator()(const UInt& A) const
        {
            ASTNode* sample  = new ASTNode(NodeType::Sample1D, return_type<TYPE>::type);
            sample->add_child(new ASTNode(NodeType::ConstVar, return_type<Buffer1D<TYPE>>::type, _id));
            sample->add_child(create_data_node(A));
            return RValue<TYPE>(sample);
        }
    
        LValue<TYPE> operator()(uint32_t A)
        {
            ASTNode* sample  = new ASTNode(NodeType::Sample1D, return_type<TYPE>::type);
            sample->add_child(new ASTNode(NodeType::Var, return_type<Buffer1D<TYPE>>::type, _id));
            sample->add_child(create_literal_node(A));
            return LValue<TYPE>(sample);
        }
    
        LValue<TYPE> operator()(const UInt& A)
        {
            ASTNode* sample  = new ASTNode(NodeType::Sample1D, return_type<TYPE>::type);
            sample->add_child(new ASTNode(NodeType::Var, return_type<Buffer1D<TYPE>>::type, _id));
            sample->add_child(create_data_node(A));
            return LValue<TYPE>(sample);
        }
    END_TYPE
    
    START_BUFFER_TYPE(Buffer2D)
        /// Prevent instantions of Buffer2Ds with invalid TYPE
        static_assert(is_data_derived<TYPE>::value, "Invalid type used in Buffer2D");
    
        /// const sampling operators
        const RValue<TYPE>  operator()(uint32_t A, uint32_t B) const
        {
            ASTNode* sample  = new ASTNode(NodeType::Sample2D, return_type<TYPE>::type);
            sample->add_child(new ASTNode(NodeType::ConstVar, return_type<Buffer2D<TYPE>>::type, _id));
            sample->add_child(create_literal_node(A));
            sample->add_child(create_literal_node(B));
            return RValue<TYPE>(sample);
        }
    
        const RValue<TYPE> operator()(const UInt& A, uint32_t B) const
        {
            ASTNode* sample  = new ASTNode(NodeType::Sample2D, return_type<TYPE>::type);
            sample->add_child(new ASTNode(NodeType::ConstVar, return_type<Buffer2D<TYPE>>::type, _id));
            sample->add_child(create_data_node(A));
            sample->add_child(create_literal_node(B));
            return RValue<TYPE>(sample);
        }
    
        const RValue<TYPE>  operator()(uint32_t A, const UInt& B) const
        {
            ASTNode* sample  = new ASTNode(NodeType::Sample2D, return_type<TYPE>::type);
            sample->add_child(new ASTNode(NodeType::ConstVar, return_type<Buffer2D<TYPE>>::type, _id));
            sample->add_child(create_literal_node(A));
            sample->add_child(create_data_node(B));
            return RValue<TYPE>(sample);
        }
    
        const RValue<TYPE> operator()(const UInt& A, const UInt& B) const
        {
            ASTNode* sample  = new ASTNode(NodeType::Sample2D, return_type<TYPE>::type);
            sample->add_child(new ASTNode(NodeType::ConstVar, return_type<Buffer2D<TYPE>>::type, _id));
            sample->add_child(create_data_node(A));
            sample->add_child(create_data_node(B));
            return RValue<TYPE>(sample);
        }
    
        const RValue<TYPE> operator()(const UInt2& VEC) const
        {
            return (*this)(VEC.X, VEC.Y);
        }
        // non-const samplign operators
        LValue<TYPE>  operator()(uint32_t A, uint32_t B)
        {
            ASTNode* sample  = new ASTNode(NodeType::Sample2D, return_type<TYPE>::type);
            sample->add_child(new ASTNode(NodeType::Var, return_type<Buffer2D<TYPE>>::type, _id));
            sample->add_child(create_literal_node(A));
            sample->add_child(create_literal_node(B));
            return LValue<TYPE>(sample);
        }
    
        LValue<TYPE> operator()(const UInt& A, uint32_t B)
        {
            ASTNode* sample  = new ASTNode(NodeType::Sample2D, return_type<TYPE>::type);
            sample->add_child(new ASTNode(NodeType::Var, return_type<Buffer2D<TYPE>>::type, _id));
            sample->add_child(create_data_node(A));
            sample->add_child(create_literal_node(B));
            return LValue<TYPE>(sample);
        }
    
        LValue<TYPE>  operator()(uint32_t A, const UInt& B)
        {
            ASTNode* sample  = new ASTNode(NodeType::Sample2D, return_type<TYPE>::type);
            sample->add_child(new ASTNode(NodeType::Var, return_type<Buffer2D<TYPE>>::type, _id));
            sample->add_child(create_literal_node(A));
            sample->add_child(create_data_node(B));
            return LValue<TYPE>(sample);
        }
    
        LValue<TYPE> operator()(const UInt& A, const UInt& B)
        {
            ASTNode* sample  = new ASTNode(NodeType::Sample2D, return_type<TYPE>::type);
            sample->add_child(new ASTNode(NodeType::Var, return_type<Buffer2D<TYPE>>::type, _id));
            sample->add_child(create_data_node(A));
            sample->add_child(create_data_node(B));
            return LValue<TYPE>(sample);
        }
    
        LValue<TYPE> operator()(const UInt2& VEC)
        {
            return (*this)(VEC.X, VEC.Y);
        }
    END_TYPE
}
