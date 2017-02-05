#pragma once

namespace Spark
{
    namespace Internal
    {
        // void type
        struct void_type
        {
            static constexpr Datatype type = Datatype(Primitive::Void, Components::None, false);
        };
        constexpr Datatype void_type::type;
    }
    typedef Internal::void_type Void;
}