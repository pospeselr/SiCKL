#pragma once

namespace spark
{
    namespace client
    {
        SPARK_NEVER_INLINE
        spark_node_t* functor_operator(spark::shared::Datatype returnType, spark_node_t* functionNode)
        {
            const auto dt = static_cast<spark_datatype_t>(returnType);
            const auto op = static_cast<spark_operator_t>(spark::shared::Operator::Call);

            auto opNode = spark_create_operator_node(dt, static_cast<spark_operator_t>(op),  THROW_ON_ERROR());
            spark_add_child_node(opNode, functionNode,  THROW_ON_ERROR());

            return opNode;
        }

        SPARK_NEVER_INLINE
        auto function_create_begin(spark::shared::Datatype returnType)
        {
            // get the root kernel node
            auto kernelRoot = spark_get_root_node( THROW_ON_ERROR());

            // verify the kernel root is our current scope
            SPARK_ASSERT(spark_peek_scope_node( THROW_ON_ERROR()) == kernelRoot);

            // create root of function Node
            const auto dt = static_cast<spark_datatype_t>(returnType);
            auto functionRoot = spark_create_function_node(dt,  THROW_ON_ERROR());
            spark_push_scope_node(functionRoot,  THROW_ON_ERROR());

            // create the parameter list
            auto parameterList = spark_create_control_node(static_cast<spark_control_t>(spark::shared::Control::ParameterList),  THROW_ON_ERROR());
            spark_add_child_node(functionRoot, parameterList,  THROW_ON_ERROR());
            spark_push_scope_node(parameterList,  THROW_ON_ERROR());

            return std::make_tuple(kernelRoot, functionRoot, parameterList);
        }

        SPARK_NEVER_INLINE
        void function_create_middle(spark_node_t* functionRoot)
        {
            spark_pop_scope_node( THROW_ON_ERROR());

            // create the body and make current scope
            auto body = spark_create_scope_block_node( THROW_ON_ERROR());
            spark_add_child_node(functionRoot, body,  THROW_ON_ERROR());
            spark_push_scope_node(body,  THROW_ON_ERROR());
        }

        SPARK_NEVER_INLINE
        void function_create_end(spark_node_t* kernelRoot, spark_node_t* functionRoot)
        {
            spark_pop_scope_node( THROW_ON_ERROR());

            // add function to kernel root
            spark_add_child_node(kernelRoot, functionRoot,  THROW_ON_ERROR());

            // done with function
            spark_pop_scope_node( THROW_ON_ERROR());
        }
    }

    template<typename> struct Function;

    template<typename RETURN, typename... PARAMS>
    struct Function<RETURN(PARAMS...)>
    {
        SPARK_FORCE_INLINE
        Function(auto func)
        {
            *this = func;
        }

        SPARK_FORCE_INLINE
        Function& operator=(auto func)
        {
            SPARK_ASSERT(this->_node == nullptr);

            function_create(func, PARAMS(nullptr)...);

            return *this;
        }

        SPARK_FORCE_INLINE
        void SetEntryPoint()
        {
            spark_node_make_entrypoint(this->_node,  THROW_ON_ERROR());
        }

        SPARK_FORCE_INLINE
        const client::rvalue<RETURN, true> operator()(const PARAMS&... params) const
        {
            auto opNode = client::functor_operator(RETURN::type, this->_node);
            function_push_param(opNode, params...);

            return client::rvalue<RETURN, true>(opNode);
        }
    private:

        SPARK_FORCE_INLINE
        void function_create(auto function_body, PARAMS&&... params)
        {
            spark_node_t* kernelRoot;
            spark_node_t* functionRoot;
            spark_node_t* parameterList;
            std::tie(kernelRoot, functionRoot, parameterList) = client::function_create_begin(RETURN::type);

            // fill out params
            function_header(parameterList, std::forward<PARAMS>(params)...);

            client::function_create_middle(functionRoot);

            // fill out body
            function_body(std::forward<PARAMS>(params)...);

            client::function_create_end(kernelRoot, functionRoot);
            this->_node = functionRoot;
        }

        // build function AST
        template<typename PARAM, typename... TAIL_PARAMS>
        SPARK_FORCE_INLINE
        void function_header(spark_node_t* parameterList, PARAM param0, TAIL_PARAMS... tailParams)
        {
            spark_add_child_node(parameterList, param0._node,  THROW_ON_ERROR());
            return function_header(parameterList, std::forward<TAIL_PARAMS>(tailParams)...);
        }

        // overload for buffer1d
        template<typename TYPE, typename... TAIL_PARAMS>
        void function_header(spark_node_t* parameterList, client::buffer1d<TYPE> param0, TAIL_PARAMS... tailParams)
        {
            spark_add_child_node(parameterList, param0.Data()._node, THROW_ON_ERROR());
            spark_add_child_node(parameterList, param0.Size._node, THROW_ON_ERROR());
            return function_header(parameterList, std::forward<TAIL_PARAMS>(tailParams)...);
        }


        SPARK_FORCE_INLINE
        void function_header(spark_node_t*) {};

        // used for invoking function
        template<typename PARAM0, typename... TAIL_PARAMS>
        SPARK_FORCE_INLINE
        void function_push_param(spark_node_t* opNode, const PARAM0& param0, const TAIL_PARAMS&... tailParams) const
        {
            SPARK_ASSERT(opNode != nullptr);
            SPARK_ASSERT(param0._node != nullptr);

            spark_add_child_node(opNode, param0._node,  THROW_ON_ERROR());

            function_push_param(opNode, tailParams...);
        }
        SPARK_FORCE_INLINE
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
            spark_begin_program( THROW_ON_ERROR());

            // create kernel AST
            auto kernelRoot = spark_create_control_node(static_cast<spark_control_t>(spark::shared::Control::Root),  THROW_ON_ERROR());
            spark_push_scope_node(kernelRoot,  THROW_ON_ERROR());

            // build the function body
            func();

            spark_pop_scope_node(THROW_ON_ERROR());

            // compile and cache program
            this->_kernel = spark_create_kernel(kernelRoot, THROW_ON_ERROR());

            printf("%s\n", spark_get_kernel_source(this->_kernel, THROW_ON_ERROR()));


            spark_end_program(THROW_ON_ERROR());
        };

        ~Kernel()
        {
            spark_destroy_kernel(this->_kernel, THROW_ON_ERROR());
        }

        void operator()(PARAMS...) const
        {

        };
    private:
        spark_kernel_t* _kernel = nullptr;
    };

    /// Return Operators

    template<typename RETURN>
    void Return(const RETURN& value)
    {
        const auto dt = static_cast<spark_datatype_t>(value.type);
        const auto op = static_cast<spark_operator_t>(spark::shared::Operator::Return);

        auto returnNode = spark_create_operator1_node(dt, op, value._node);

        auto currentScope = spark_peek_scope_node( THROW_ON_ERROR());
        spark_add_child_node(currentScope, returnNode,  THROW_ON_ERROR());
    }

    void Return()
    {
        const auto dt = static_cast<spark_datatype_t>(spark::shared::Datatype(spark::shared::Primitive::Void, spark::shared::Components::None, false));
        const auto op = static_cast<spark_operator_t>(spark::shared::Operator::Return);

        auto returnNode = spark_create_operator_node(dt, op,  THROW_ON_ERROR());

        auto currentScope = spark_peek_scope_node( THROW_ON_ERROR());
        spark_add_child_node(currentScope, returnNode,  THROW_ON_ERROR());
    }
}

