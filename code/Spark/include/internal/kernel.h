#pragma once

namespace Spark
{
	/// Function

	// no-op ends recursion
	void function_push_param(Node*) {};

	template<typename PARAM0, typename... TAIL_PARAMS>
	void function_push_param(Node* opNode, const PARAM0& param0, const TAIL_PARAMS&... tailParams)
	{
		SPARK_ASSERT(opNode != nullptr);
		SPARK_ASSERT(param0._node != nullptr);

		spark_add_child_node(opNode, param0._node);

		function_push_param(opNode, tailParams...);
	}

	template<typename RETURN, typename... PARAMS>
	struct Function
	{
		Function(Node* functionRoot) : _node(functionRoot) {};

		const rvalue<RETURN, true> operator()(const PARAMS&... params)
		{
			Node* opNode = spark_create_operator_node(RETURN::type, Operator::Call);
			spark_add_child_node(opNode, this->_node);
			function_push_param(opNode, params...);

			return rvalue<RETURN, true>(opNode);
		}
	private:
		Node* _node;
	};

	template<> template<typename... PARAMS>
	struct Function<Void, PARAMS...>
	{
		Function(Node* functionRoot) : _node(functionRoot) {};

		const Void operator()(const PARAMS&... params)
		{
			Node* opNode = spark_create_operator_node(DataType::Void, Operator::Call);
			spark_add_child_node(opNode, this->_node);
			function_push_param(opNode, params...);

			Node* currentScope = spark_peek_scope_node();
			spark_add_child_node(currentScope, opNode);
		}
	private:
		Node* _node;
	};

	// no-op ends recursion
	void function_header(Node*) {};

	template<typename PARAM, typename... TAIL_PARAMS>
	void function_header(Node* parameterList, PARAM param0, TAIL_PARAMS... tailParams)
	{
		spark_add_child_node(parameterList, param0._node);
		return function_header(parameterList, std::forward<TAIL_PARAMS>(tailParams)...);
	}

	template<typename FUNC, typename... PARAMS>
	Node* create_function(symbolid_t id, FUNC&& function_body, PARAMS... params)
	{
        // get the root kernel node
        Node* kernelRoot = spark_get_root_node();

		// create root of function Node
		Node* functionRoot = spark_create_function_node(id);
        spark_push_scope_node(functionRoot);

		// create the parameter list
		Node* parameterList = spark_create_control_node(Control::ParameterList);
        spark_add_child_node(functionRoot, parameterList);
        spark_push_scope_node(parameterList);

		// fill out params
		function_header(parameterList, std::forward<PARAMS>(params)...);
		spark_pop_scope_node();

		// create the body and make current scope
		Node* body = spark_create_control_node(Control::ScopeBlock);
        spark_add_child_node(functionRoot, body);
        spark_push_scope_node(body);

		// fill out body
		(void)function_body(std::forward<PARAMS>(params)...);
		spark_pop_scope_node();

        // add function to kernel root
        spark_add_child_node(kernelRoot, functionRoot);

		// done with function
		spark_pop_scope_node();

		// return a copy of the functionRoot (sans children) to pass
		// around to callers

		Node* result = spark_create_function_node(functionRoot->_function.id);
		return result;
	}

	template<typename RETURN>
	void Return(const RETURN& value)
	{
		Node* returnNode = spark_create_operator1_node(value.type, Operator::Return, value._node);

		Node* currentScope = spark_peek_scope_node();
		spark_add_child_node(currentScope, returnNode);
	}

	void Return()
	{
		Node* returnNode = spark_create_operator_node(DataType::Void, Operator::Return);

		Node* currentScope = spark_peek_scope_node();
		spark_add_child_node(currentScope, returnNode);
	}

	template<typename RETURN, typename... PARAMS>
	auto make_function(auto&& func)
	{
		symbolid_t id = spark_next_symbol();
		return Function<RETURN, PARAMS...>(create_function(id, func, PARAMS(nullptr)...));
	}

	/// Kernel

	template<typename... PARAMS>
	struct Kernel
	{
		Kernel(Node* kernelRoot) : _node(kernelRoot) {};

		void operator()() const
		{

		};
	private:
		Node* _node;
	};

	// no-op ends recursion
	void kernel_header(Node*) {};

	template<typename PARAM, typename... TAIL_PARAMS>
	void kernel_header(Node* parameterList, PARAM param0, TAIL_PARAMS... tailParams)
	{
		spark_add_child_node(parameterList, param0._node);
		return kernel_header(parameterList, std::forward<TAIL_PARAMS>(tailParams)...);
	}

	template<typename FUNC, typename... PARAMS>
	Node* create_kernel(symbolid_t id, FUNC&& kernel_body, PARAMS... params)
	{
		// create root of program
		Node* kernelRoot = spark_create_control_node(Control::Root);
		spark_push_scope_node(kernelRoot);

        // create kernel function root
        Node* functionRoot = spark_create_function_node(id);
        spark_push_scope_node(functionRoot);

		// create kernel parameter list
		Node* parameterList = spark_create_control_node(Control::ParameterList);
        spark_add_child_node(functionRoot, parameterList);
        spark_push_scope_node(parameterList);

		// fill out params
		kernel_header(parameterList, std::forward<PARAMS>(params)...);
		spark_pop_scope_node();

		// create the kernel body and make current scope
		Node* body = spark_create_control_node(Control::ScopeBlock);
        spark_add_child_node(functionRoot, body);
        spark_push_scope_node(body);

		(void)kernel_body(std::forward<PARAMS>(params)...);
		spark_pop_scope_node();

        // add function to kernel root
        spark_add_child_node(kernelRoot, functionRoot);

        // done with kernel function
        spark_pop_scope_node();

		// done with program
		spark_pop_scope_node();
		return kernelRoot;
	}

	template<typename... PARAMS>
	auto make_kernel(auto&& func)
	{
		spark_begin_program();
        symbolid_t id = spark_next_symbol();
		Node* kernelAst = create_kernel(id, func, PARAMS(nullptr)...);

        // print AST
        const auto len = spark_node_to_text(kernelAst, nullptr, 0);
        unique_ptr<char[]> buff(new char[len]);
        spark_node_to_text(kernelAst, buff.get(), len);

        printf("%s\n", buff.get());

        spark_end_program();
		return Kernel<PARAMS...>(kernelAst);
	}
}