#pragma once

namespace SiCKL
{
    class Compiler
    {
    public:
        static sickl_int Build(SiCKL::Source& source, Program& program);
    };
}
