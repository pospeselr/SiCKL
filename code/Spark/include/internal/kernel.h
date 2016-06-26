#pragma once

#include <utility>

namespace Spark
{
	template<typename RETURN, typename... PARAMS>
	struct Function
	{
		Function(Node* functionRoot) : _node(functionRoot) {};

		// no-op ends recursion
		void push_param(Node*) {};

		template<typename PARAM0, typename... TAIL_PARAMS>
		void push_param(Node* opNode, const PARAM0& param0, const TAIL_PARAMS&... tailParams)
		{
			SPARK_ASSERT(opNode != nullptr);
			SPARK_ASSERT(param0._node != nullptr);

			spark_add_child_node(opNode, param0._node);

			push_param(opNode, tailParams...);
		}

		const rvalue<RETURN> operator()(const PARAMS&... params)
		{
			Node* opNode = spark_create_operator_node(DataType::Void, Operator::Call);
			spark_add_child_node(opNode, this->_node);
			push_param(opNode, params...);

			return rvalue<RETURN>(opNode);
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
	Node* create_function(FUNC&& function_body, PARAMS... params)
	{
		// create root of function Node
		Node* functionRoot = spark_create_function_node(spark_next_symbol());
		spark_push_scope_node(functionRoot);

		// create the parameter list
		Node* parameterList = spark_create_control_node(Control::ParameterList);
		spark_push_scope_node(parameterList);

		// fill out params
		function_header(parameterList, std::forward<PARAMS>(params)...);
		spark_pop_scope_node();

		// create the body and make current scope
		Node* body = spark_create_control_node(Control::ScopeBlock);
		spark_push_scope_node(body);

		// fill out body
		function_body(std::forward<PARAMS>(params)...);
		spark_pop_scope_node();

		// done with function
		spark_pop_scope_node();

		// return a copy of the functionRoot (sans children) to pass
		// around to callers

		Node* result = spark_create_function_node(functionRoot->_function.id);
		return result;
	}

	template<typename RETURN, typename... PARAMS>
	Function<RETURN, PARAMS...> make_function(auto&& func)
	{
		return Function<RETURN, PARAMS...>(create_function(func, PARAMS(nullptr)...));
	}
}