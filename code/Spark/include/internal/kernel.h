#pragma once

#define Return (void)

namespace Spark
{
	template<typename... AllParams>
	struct Function
	{
		Function(Node* functionRoot) : _node(functionRoot) {};

		// no-op ends recursion
		void push_param(Node*) {};

		template<typename Param0, typename... Params>
		void push_param(Node* opNode, const Param0& param0, const Params&... params)
		{
			SPARK_ASSERT(opNode != nullptr);
			SPARK_ASSERT(param0._node != nullptr);

			spark_add_child_node(opNode, param0._node);

			push_param(opNode, params...);
		}

		void operator()(AllParams&... allParams)
		{
			Node* opNode = spark_create_operator_node(DataType::Void, Operator::Call);
			spark_add_child_node(opNode, this->_node);
			push_param(opNode, allParams...);

			spark_push_scope_node(opNode);
		}
	private:
		Node* _node;
	};

	// no-op ends recursion
	void function_header(Node*) {};

	template<typename Param0, typename... Params>
	void function_header(Node* parameterList, Param0&& param0, Params&&... params)
	{
		spark_add_child_node(parameterList, param0._node);
		return function_header(parameterList, params...);
	}

	template<typename Func, typename... Params>
	Node* create_function(Func&& function_body, Params&&... params)
	{
		// create root of function node
		Node* functionRoot = spark_create_function_node(spark_next_symbol());
		spark_push_scope_node(functionRoot);

		// create the parameter list
		Node* parameterList = spark_create_control_node(Control::ParameterList);
		spark_push_scope_node(parameterList);

		// fill out params
		function_header(parameterList, params...);
		spark_pop_scope_node();

		// create the body and make current scope
		Node* body = spark_create_control_node(Control::ScopeBlock);
		spark_push_scope_node(body);

		// fill out body
		function_body(params...);
		spark_pop_scope_node();

		// done with function
		spark_pop_scope_node();

		return functionRoot;
	}

	template<typename... Params>
	Function<Params...> make_function(auto&& func)
	{
		return Function<Params...>(create_function(func, Params(nullptr)...));
	}
}