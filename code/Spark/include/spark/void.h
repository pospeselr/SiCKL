#pragma once

namespace spark
{
    namespace client
    {
        // void type
        struct void_type
        {
            static constexpr spark::shared::Datatype type = spark::shared::Datatype(spark::shared::Primitive::Void, spark::shared::Components::None, false);
        };
        constexpr spark::shared::Datatype void_type::type;
    }
    typedef client::void_type Void;
}