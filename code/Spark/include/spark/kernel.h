#pragma once

namespace Spark
{
    template<typename> struct Function;

    template<typename RETURN, typename... PARAMS>
    struct Function<RETURN(PARAMS...)>
    {
        Function<RETURN(PARAMS...)>(auto func)
        {
            const symbolid_t id = spark_next_symbol();
            function_create(func, id, PARAMS(nullptr)...);
        }

        const rvalue<RETURN, true> operator()(const PARAMS&... params) const
        {
            Node* opNode = spark_create_operator_node(RETURN::type, Operator::Call);
            spark_add_child_node(opNode, this->_node);
            function_push_param(opNode, params...);

            return rvalue<RETURN, true>(opNode);
        }
    private:

        void function_create(auto function_body, symbolid_t id, PARAMS&&... params)
        {
            // get the root kernel node
            Node* kernelRoot = spark_get_root_node();

            // verify the kernel root is our current scope
            SPARK_ASSERT(spark_peek_scope_node() == kernelRoot);

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

            // use a copy of the functionRoot (sans children) to pass
            // around to callers

            this->_node = spark_create_function_node(id);
        }

        // build function AST
        template<typename PARAM, typename... TAIL_PARAMS>
        void function_header(Node* parameterList, PARAM param0, TAIL_PARAMS... tailParams)
        {
            spark_add_child_node(parameterList, param0._node);
            return function_header(parameterList, std::forward<TAIL_PARAMS>(tailParams)...);
        }
        void function_header(Node*) {};

        // used for invoking function
        template<typename PARAM0, typename... TAIL_PARAMS>
        void function_push_param(Node* opNode, const PARAM0& param0, const TAIL_PARAMS&... tailParams) const
        {
            SPARK_ASSERT(opNode != nullptr);
            SPARK_ASSERT(param0._node != nullptr);

            spark_add_child_node(opNode, param0._node);

            function_push_param(opNode, tailParams...);
        }
        void function_push_param(Node*) const {};

        Node* _node = nullptr;
    };

    /// Kernel
    template<typename> struct Kernel;

    template<typename... PARAMS>
    struct Kernel<void(PARAMS...)>
    {
        Kernel(auto func)
        {
            spark_begin_program();
            auto cleanup = scope_exit(::spark_end_program);

            // create kernel AST
            Node* kernelRoot = spark_create_control_node(Control::Root);
            spark_push_scope_node(kernelRoot);

            // build the function body
            func();

            spark_pop_scope_node();

            // compile and cache program

            const auto len = spark_node_to_text(kernelRoot, nullptr, 0);
            unique_ptr<char[]> buff(new char[len]);
            spark_node_to_text(kernelRoot, buff.get(), len);

            printf("%s\n", buff.get());
        };

        void operator()(PARAMS...) const
        {

        };
    };

    /// Return Operators

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
}