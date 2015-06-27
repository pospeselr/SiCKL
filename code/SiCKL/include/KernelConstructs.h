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
}
