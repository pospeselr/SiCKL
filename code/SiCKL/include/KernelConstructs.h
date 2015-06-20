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
        {
            T* _this = (T*)this;
            _this->begin(args...);
        }
     
        void finish()
        {
            Source::end_block();
            _finished = true;
        }
        
        bool finished() const {return _finished;}
    private:
        bool _finished;
    };
    
    // construct for entering KernelMain
    struct kernelmain_construct : public SiCKL::construct<SiCKL::kernelmain_construct>
    {
        void begin();
    };
    
    // construct for entering If statements
    struct if_construct : public SiCKL::construct<SiCKL::if_construct, const SiCKL::Bool&>
    {
        using SiCKL::construct<SiCKL::if_construct, const SiCKL::Bool&>::construct;
        void begin(const SiCKL::Bool&);
    };
    
    // construct for entering ElseIf statements
    struct elseif_construct : public SiCKL::construct<SiCKL::elseif_construct, const SiCKL::Bool&>
    {
        using SiCKL::construct<SiCKL::elseif_construct, const SiCKL::Bool&>::construct;
        void begin(const SiCKL::Bool&);
    };
    
    struct else_construct : public SiCKL::construct<SiCKL::else_construct>
    {
        void begin();
    };
    
    struct while_construct : public SiCKL::construct<SiCKL::while_construct, const SiCKL::Bool&>
    {
        using SiCKL::construct<SiCKL::while_construct, const SiCKL::Bool&>::construct;
        void begin(const SiCKL::Bool&);
    };
    
    // forinrange_construct extends SiCKL::Int so that it can be used as the Int variable
    struct forinrange_construct : public SiCKL::Int
                                , public SiCKL::construct<SiCKL::forinrange_construct, int32_t, int32_t>
    {
        using SiCKL::construct<SiCKL::forinrange_construct, int32_t, int32_t>::construct;
        void begin(int32_t, int32_t);
    };
}
