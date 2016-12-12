#pragma once

namespace Spark
{
    template<typename> struct Function;

    template<typename RETURN, typename... PARAMS>
    struct Function<RETURN(PARAMS...)>
    {
        Function(auto func)
        {
            *this = func;
        }

        Function& operator=(auto func)
        {
            SPARK_ASSERT(this->_node == nullptr);

            const symbolid_t id = spark_next_symbol(Spark::Internal::ThrowOnError());
            function_create(func, id, PARAMS(nullptr)...);

            return *this;
        }

        void SetEntryPoint()
        {
            spark_node_make_entrypoint(this->_node, Spark::Internal::ThrowOnError());
        }

        const rvalue<RETURN, true> operator()(const PARAMS&... params) const
        {
            Node* opNode = spark_create_operator_node(RETURN::type, Operator::Call, Spark::Internal::ThrowOnError());
            const symbolid_t id = spark_node_get_function_id(this->_node, Spark::Internal::ThrowOnError());
            spark_add_child_node(opNode, spark_create_function_node(id, Spark::Internal::ThrowOnError()), Spark::Internal::ThrowOnError());
            function_push_param(opNode, params...);

            return rvalue<RETURN, true>(opNode);
        }
    private:

        void function_create(auto function_body, symbolid_t id, PARAMS&&... params)
        {
            // get the root kernel node
            Node* kernelRoot = spark_get_root_node(Spark::Internal::ThrowOnError());

            // verify the kernel root is our current scope
            SPARK_ASSERT(spark_peek_scope_node(Spark::Internal::ThrowOnError()) == kernelRoot);

            // create root of function Node
            Node* functionRoot = spark_create_function_node(id, Spark::Internal::ThrowOnError());
            spark_push_scope_node(functionRoot, Spark::Internal::ThrowOnError());

            // create the parameter list
            Node* parameterList = spark_create_control_node(Control::ParameterList, Spark::Internal::ThrowOnError());
            spark_add_child_node(functionRoot, parameterList, Spark::Internal::ThrowOnError());
            spark_push_scope_node(parameterList, Spark::Internal::ThrowOnError());

            // fill out params
            function_header(parameterList, std::forward<PARAMS>(params)...);
            spark_pop_scope_node(Spark::Internal::ThrowOnError());

            // create the body and make current scope
            Node* body = spark_create_control_node(Control::ScopeBlock, Spark::Internal::ThrowOnError());
            spark_add_child_node(functionRoot, body, Spark::Internal::ThrowOnError());
            spark_push_scope_node(body, Spark::Internal::ThrowOnError());

            // fill out body
            (void)function_body(std::forward<PARAMS>(params)...);
            spark_pop_scope_node(Spark::Internal::ThrowOnError());

            // add function to kernel root
            spark_add_child_node(kernelRoot, functionRoot, Spark::Internal::ThrowOnError());

            // done with function
            spark_pop_scope_node(Spark::Internal::ThrowOnError());

            this->_node = functionRoot;
        }

        // build function AST
        template<typename PARAM, typename... TAIL_PARAMS>
        void function_header(Node* parameterList, PARAM param0, TAIL_PARAMS... tailParams)
        {
            spark_add_child_node(parameterList, param0._node, Spark::Internal::ThrowOnError());
            return function_header(parameterList, std::forward<TAIL_PARAMS>(tailParams)...);
        }
        void function_header(Node*) {};

        // used for invoking function
        template<typename PARAM0, typename... TAIL_PARAMS>
        void function_push_param(Node* opNode, const PARAM0& param0, const TAIL_PARAMS&... tailParams) const
        {
            SPARK_ASSERT(opNode != nullptr);
            SPARK_ASSERT(param0._node != nullptr);

            spark_add_child_node(opNode, param0._node, Spark::Internal::ThrowOnError());

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
            spark_begin_program(Spark::Internal::ThrowOnError());
            auto cleanup = scope_exit(
                []
                {
                    spark_end_program(Spark::Internal::ThrowOnError());
                });

            // create kernel AST
            Node* kernelRoot = spark_create_control_node(Control::Root, Spark::Internal::ThrowOnError());
            spark_push_scope_node(kernelRoot, Spark::Internal::ThrowOnError());

            // build the function body
            func();

            spark_pop_scope_node(Spark::Internal::ThrowOnError());

            // compile and cache program

            const auto len = spark_node_to_text(kernelRoot, nullptr, 0, Spark::Internal::ThrowOnError());
            unique_ptr<char[]> buff(new char[len]);
            spark_node_to_text(kernelRoot, buff.get(), len, Spark::Internal::ThrowOnError());

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

        Node* currentScope = spark_peek_scope_node(Spark::Internal::ThrowOnError());
        spark_add_child_node(currentScope, returnNode, Spark::Internal::ThrowOnError());
    }

    void Return()
    {
        Node* returnNode = spark_create_operator_node(DataType::Void, Operator::Return, Spark::Internal::ThrowOnError());

        Node* currentScope = spark_peek_scope_node(Spark::Internal::ThrowOnError());
        spark_add_child_node(currentScope, returnNode, Spark::Internal::ThrowOnError());
    }
}