#pragma once

namespace Spark
{
    using namespace Internal;

    // void type
    struct Void
    {
        static constexpr Datatype type = Datatype(Primitive::Void, Components::None, false);
    };
    constexpr Datatype Void::type;
}