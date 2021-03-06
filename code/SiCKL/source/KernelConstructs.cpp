#include "SiCKL.h"
#include "SiCKLUndef.h"

namespace SiCKL
{
    void function_body_construct::begin()
    {
		// we need two end counts to also close out the root Function node
        _end_count = 2;
        ASTNode* main_block = new ASTNode(NodeType::Body, DataType::Void);

        Source::start_block(main_block);
    }

    void if_construct::begin(const SiCKL::Bool& b)
    {
        ASTNode* if_block = new ASTNode(NodeType::If, DataType::Void);
        ASTNode* condition = create_value_node(b);

        if_block->add_child(condition);

        Source::start_block(if_block);
    }

    void elseif_construct::begin(const SiCKL::Bool& b)
    {
        // ensure the previous node is an If block
        {
            const size_t& count = Source::_current_block->_count;
            SICKL_ASSERT(count > 0);
            const NodeType_t& prev_type = Source::_current_block->_children[count - 1]->_node_type;
            SICKL_ASSERT(prev_type == NodeType::If || prev_type == NodeType::ElseIf);
        }

        // add ElseIf block
        ASTNode* elseif_block = new ASTNode(NodeType::ElseIf, DataType::Void);
        ASTNode* condition = create_value_node(b);

        elseif_block->add_child(condition);

        Source::start_block(elseif_block);
    }

    void else_construct::begin()
    {
        // ensure the previous node is an If or an ElseIf block
        {
            const size_t& count = Source::_current_block->_count;
            SICKL_ASSERT(count > 0);
            const NodeType_t& prev_type = Source::_current_block->_children[count - 1]->_node_type;
            SICKL_ASSERT(prev_type == NodeType::If || prev_type == NodeType::ElseIf);
        }

        ASTNode* else_block = new ASTNode(NodeType::Else, DataType::Void);

        Source::start_block(else_block);
    }

    void while_construct::begin(const SiCKL::Bool& b)
    {
        ASTNode* while_block = new ASTNode(NodeType::While, DataType::Void);
        ASTNode* condition = create_value_node(b);

        while_block->add_child(condition);

        Source::start_block(while_block);
    }

    void forinrange_construct::begin(const int32_t from, const int32_t to)
    {
        const SiCKL::Int& it = *this;
        SICKL_ASSERT(it._id == invalid_symbol);
        // give this guy a symbol id
        it._id = Source::next_symbol();

        ASTNode* forinrange_block = new ASTNode(NodeType::ForInRange, DataType::Void);

        forinrange_block->add_child(create_value_node(it));
        forinrange_block->add_child(create_literal_node(from));
        forinrange_block->add_child(create_literal_node(to));

        Source::start_block(forinrange_block);
    }
}
