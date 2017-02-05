#pragma once

namespace Spark
{
    namespace Internal
    {
        inline
        __attribute__ ((noinline))
        spark_node_t* functor_operator(Datatype returnType, spark_node_t* functionNode)
        {
            const auto dt = static_cast<spark_datatype_t>(returnType);
            const auto op = static_cast<spark_operator_t>(Operator::Call);

            auto opNode = spark_create_operator_node(dt, static_cast<spark_operator_t>(op), Spark::Internal::ThrowOnError());
            spark_add_child_node(opNode, functionNode, Spark::Internal::ThrowOnError());

            return opNode;
        }

        inline
        __attribute__ ((noinline))
        auto function_create_begin(Datatype returnType)
        {
            // get the root kernel node
            auto kernelRoot = spark_get_root_node(Spark::Internal::ThrowOnError());

            // verify the kernel root is our current scope
            SPARK_ASSERT(spark_peek_scope_node(Spark::Internal::ThrowOnError()) == kernelRoot);

            // create root of function Node
            const auto dt = static_cast<spark_datatype_t>(returnType);
            auto functionRoot = spark_create_function_node(dt, Spark::Internal::ThrowOnError());
            spark_push_scope_node(functionRoot, Spark::Internal::ThrowOnError());

            // create the parameter list
            auto parameterList = spark_create_control_node(static_cast<spark_control_t>(Control::ParameterList), Spark::Internal::ThrowOnError());
            spark_add_child_node(functionRoot, parameterList, Spark::Internal::ThrowOnError());
            spark_push_scope_node(parameterList, Spark::Internal::ThrowOnError());

            return std::make_tuple(kernelRoot, functionRoot, parameterList);
        }

        inline
        __attribute__ ((noinline))
        void function_create_middle(spark_node_t* functionRoot)
        {
            spark_pop_scope_node(Spark::Internal::ThrowOnError());

            // create the body and make current scope
            auto body = spark_create_scope_block_node(Spark::Internal::ThrowOnError());
            spark_add_child_node(functionRoot, body, Spark::Internal::ThrowOnError());
            spark_push_scope_node(body, Spark::Internal::ThrowOnError());
        }

        inline
        __attribute__ ((noinline))
        void function_create_end(spark_node_t* kernelRoot, spark_node_t* functionRoot)
        {
            spark_pop_scope_node(Spark::Internal::ThrowOnError());

            // add function to kernel root
            spark_add_child_node(kernelRoot, functionRoot, Spark::Internal::ThrowOnError());

            // done with function
            spark_pop_scope_node(Spark::Internal::ThrowOnError());
        }
    }
    using namespace Internal;

    template<typename> struct Function;

    template<typename RETURN, typename... PARAMS>
    struct Function<RETURN(PARAMS...)>
    {
        __attribute__ ((always_inline))
        Function(auto func)
        {
            *this = func;
        }

        __attribute__ ((always_inline))
        Function& operator=(auto func)
        {
            SPARK_ASSERT(this->_node == nullptr);

            function_create(func, PARAMS(nullptr)...);

            return *this;
        }

        __attribute__ ((always_inline))
        void SetEntryPoint()
        {
            spark_node_make_entrypoint(this->_node, Spark::Internal::ThrowOnError());
        }

        __attribute__ ((always_inline))
        const rvalue<RETURN, true> operator()(const PARAMS&... params) const
        {
            auto opNode = Internal::functor_operator(RETURN::type, this->_node);
            function_push_param(opNode, params...);

            return rvalue<RETURN, true>(opNode);
        }
    private:

        inline
        __attribute__ ((always_inline))
        void function_create(auto function_body, PARAMS&&... params)
        {
            spark_node_t* kernelRoot;
            spark_node_t* functionRoot;
            spark_node_t* parameterList;
            std::tie(kernelRoot, functionRoot, parameterList) = Internal::function_create_begin(RETURN::type);

            // fill out params
            function_header(parameterList, std::forward<PARAMS>(params)...);

            Internal::function_create_middle(functionRoot);

            // fill out body
            function_body(std::forward<PARAMS>(params)...);

            Internal::function_create_end(kernelRoot, functionRoot);
            this->_node = functionRoot;
        }

        // build function AST
        template<typename PARAM, typename... TAIL_PARAMS>
        __attribute__ ((always_inline))
        void function_header(spark_node_t* parameterList, PARAM param0, TAIL_PARAMS... tailParams)
        {
            spark_add_child_node(parameterList, param0._node, Spark::Internal::ThrowOnError());
            return function_header(parameterList, std::forward<TAIL_PARAMS>(tailParams)...);
        }
        __attribute__ ((always_inline))
        void function_header(spark_node_t*) {};

        // used for invoking function
        template<typename PARAM0, typename... TAIL_PARAMS>
        __attribute__ ((always_inline))
        void function_push_param(spark_node_t* opNode, const PARAM0& param0, const TAIL_PARAMS&... tailParams) const
        {
            SPARK_ASSERT(opNode != nullptr);
            SPARK_ASSERT(param0._node != nullptr);

            spark_add_child_node(opNode, param0._node, Spark::Internal::ThrowOnError());

            function_push_param(opNode, tailParams...);
        }
        __attribute__ ((always_inline))
        void function_push_param(spark_node_t*) const {};

        spark_node_t* _node = nullptr;
    };

    /// Kernel
    template<typename> struct Kernel;

    template<typename... PARAMS>
    struct Kernel<Void(PARAMS...)>
    {
        Kernel(auto func)
        {
            spark_begin_program(Spark::Internal::ThrowOnError());

            // create kernel AST
            auto kernelRoot = spark_create_control_node(static_cast<spark_control_t>(Internal::Control::Root), Spark::Internal::ThrowOnError());
            spark_push_scope_node(kernelRoot, Spark::Internal::ThrowOnError());

            // build the function body
            func();

            spark_pop_scope_node(Spark::Internal::ThrowOnError());

            // compile and cache program

            {
                const auto len = spark_node_to_text(kernelRoot, nullptr, 0, Spark::Internal::ThrowOnError());
                unique_ptr<char[]> buff(new char[len]);
                spark_node_to_text(kernelRoot, buff.get(), len, Spark::Internal::ThrowOnError());

                printf("%s\n", buff.get());
            }

            {
                const auto len = spark_node_to_opencl(kernelRoot, nullptr, 0, Spark::Internal::ThrowOnError());
                unique_ptr<char[]> buff(new char[len]);
                spark_node_to_opencl(kernelRoot, buff.get(), len, Spark::Internal::ThrowOnError());

                printf("%s\n", buff.get());
            }

            spark_end_program(Spark::Internal::ThrowOnError());
        };

        void operator()(PARAMS...) const
        {

        };
    };

    /// Return Operators

    template<typename RETURN>
    void Return(const RETURN& value)
    {
        const auto dt = static_cast<spark_datatype_t>(value.type);
        const auto op = static_cast<spark_operator_t>(Operator::Return);

        auto returnNode = spark_create_operator1_node(dt, op, value._node);

        auto currentScope = spark_peek_scope_node(Spark::Internal::ThrowOnError());
        spark_add_child_node(currentScope, returnNode, Spark::Internal::ThrowOnError());
    }

    void Return()
    {
        const auto dt = static_cast<spark_datatype_t>(Datatype(Primitive::Void, Components::None, false));
        const auto op = static_cast<spark_operator_t>(Operator::Return);

        auto returnNode = spark_create_operator_node(dt, op, Spark::Internal::ThrowOnError());

        auto currentScope = spark_peek_scope_node(Spark::Internal::ThrowOnError());
        spark_add_child_node(currentScope, returnNode, Spark::Internal::ThrowOnError());
    }
}

