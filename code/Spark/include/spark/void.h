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
    }
    typedef client::void_type Void;
}