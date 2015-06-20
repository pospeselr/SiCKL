#include "SiCKL.h"

#undef If
#undef ElseIf
#undef Else
#undef EndIf
#undef While
#undef ForInRange

namespace SiCKL
{
	ASTNode* Source::_root = nullptr;
	ASTNode* Source::_current_block = nullptr;
	std::vector<ASTNode*> Source::_block_stack;
    symbol_id_t Source::_next_symbol(0);

	Source::Source()
		: _my_ast(nullptr)
		, _symbol_count(-1)
	{
		
	}

	Source::~Source()
	{
		if(_my_ast != nullptr)
		{
			delete _my_ast;
		}

		_symbol_count = -1;
	}

	void Source::initialize()
	{
		delete _my_ast;
		_my_ast = nullptr;
		
		_root = nullptr;
		_current_block = nullptr;
		_block_stack.clear();
		_next_symbol = 0;

		_symbol_count = -1;

		// fill in static data
		_my_ast = new ASTNode(NodeType::Program, ReturnType::Void);
		_root = _my_ast;
		_current_block = _root;
		_block_stack.push_back(_root);
	}

	void Source::finalize()
	{
		_symbol_count = _next_symbol;

		// copy over the node ptr
		_my_ast = _root;
		// reset the static guys
		_root = nullptr;
		_current_block = nullptr;
		_block_stack.clear();
		_next_symbol = 0;
	}
	
	symbol_id_t Source::next_symbol()
	{
        return _next_symbol = (_next_symbol + 1);
	}
	
	symbol_id_t Source::current_symbol()
	{
        return _next_symbol;
	}
	
	void Source::start_block(ASTNode* block)
	{
        _current_block->add_child(block);

        _block_stack.push_back(block);
        _current_block = block;    
	}
	
	void Source::add_to_current_block(ASTNode* node)
	{
        _current_block->add_child(node);
	}
	
	void Source::end_block()
	{
        _block_stack.pop_back();
        _current_block = _block_stack.back();	
	}
}

