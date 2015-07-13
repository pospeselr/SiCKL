#pragma once

namespace SiCKL
{
    // used in conjunction with the MAKE_CONSTRUCT_* macros
    // used for making the 'While,' 'If,' etc using the for loop trick:
    // http://www.codeproject.com/Articles/12362/A-quot-synchronized-quot-statement-for-C-like-in-J
    template<typename T, typename... Args>
    struct construct
    {
        construct(Args... args)
        : _finished(false)
        , _end_count(1)
        {
            T* _this = (T*)this;
            _this->begin(args...);
        }

        void finish()
        {
            for(int k = 0; k < _end_count; k++)
            {
                Source::end_block();
            }
            _finished = true;
        }

        bool finished() const {return _finished;}
    protected:
        bool _finished;
        int _end_count;
    };

    // construct for entering KernelMain
    struct function_body_construct : public SiCKL::construct<SiCKL::function_body_construct>
    {
        void begin();
    };

    // construct for entering If statements
    struct if_construct : public SiCKL::construct<SiCKL::if_construct, const SiCKL::Bool&>
    {
        if_construct(const SiCKL::Bool& b) : construct<SiCKL::if_construct, const SiCKL::Bool&>(b) {}
        void begin(const SiCKL::Bool&);
    };

    // construct for entering ElseIf statements
    struct elseif_construct : public SiCKL::construct<SiCKL::elseif_construct, const SiCKL::Bool&>
    {
        elseif_construct(const SiCKL::Bool& b) : construct<SiCKL::elseif_construct, const SiCKL::Bool&>(b) {}
        void begin(const SiCKL::Bool&);
    };

    struct else_construct : public SiCKL::construct<SiCKL::else_construct>
    {
        else_construct() : construct<SiCKL::else_construct>() {}
        void begin();
    };

    struct while_construct : public SiCKL::construct<SiCKL::while_construct, const SiCKL::Bool&>
    {
        while_construct(const SiCKL::Bool& b) : construct<SiCKL::while_construct, const SiCKL::Bool&>(b) {}
        void begin(const SiCKL::Bool&);
    };

    // forinrange_construct extends SiCKL::Int so that it can be used as the Int variable
    struct forinrange_construct : public SiCKL::Int
                                , public SiCKL::construct<SiCKL::forinrange_construct, const int32_t, const int32_t>
    {
        forinrange_construct(const int32_t from, const int32_t to) : construct<SiCKL::forinrange_construct, const int32_t, const int32_t>(from, to) {}
        void begin(const int32_t, const int32_t);
    };
    
    template<typename T>
    struct function_parameter_generator
    {
        function_parameter_generator(bool in_finished)
        : _finished(in_finished)
        { 

        }
    
        function_parameter_generator& operator++()
        {
            SICKL_ASSERT(_finished == false);
            _finished = true;
            return *this;
        }
        
        bool operator==(const function_parameter_generator& that)
        {
            return this->_finished == that._finished;
        }
        
        bool operator!=(const function_parameter_generator& that)
        {
            return this->_finished != that._finished;
        }
    
        T operator*()
        {
            const symbol_id_t id = Source::next_symbol();
            T t(id, nullptr);
            ASTNode* param = new ASTNode(NodeType::Var, return_type<T>::type, id);
            Source::add_to_current_block(param);
            return t;
        }
    
    private:
        bool _finished;
    };
    
    template<typename T>
    struct function_parameter
    {
        function_parameter_generator<T> begin()
        {
            return function_parameter_generator<T>(false);
        }
        
        function_parameter_generator<T> end()
        {
            return function_parameter_generator<T>(true);
        }
    };
    
    struct return_statement
    {
        template<typename T>
        void operator=(const T& val)
        {
            const ASTNode* func = Source::_current_function;
            // verify we're returning the correct type
            SICKL_ASSERT(return_type<T>::type == func->_return_type);
            
            // create new 'Return' ASTNode and populate with our val
            
            ASTNode* ret = create_return_node(return_type<T>::type);
            ret->add_child(create_value_node(val));
            
            Source::add_to_current_block(ret);
        }
    };
}
