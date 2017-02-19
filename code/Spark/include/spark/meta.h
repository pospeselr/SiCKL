#pragma once

namespace spark
{
    namespace client
    {
        template<typename T> struct is_scalar_type {const static bool value = false;};
        template<typename T> struct is_vector2_type {const static bool value = false;};

        /// Type Translations

        // gets the equivalent integer type (used for comparison operators)
        template<typename T> struct type_to_signed_int {typedef void type;};

    #define MAKE_TYPE_TO_INT(TYPE, INT_TYPE)\
        template<> struct type_to_signed_int<TYPE> {typedef INT_TYPE type;};\

        MAKE_TYPE_TO_INT(int8_t, int8_t);
        MAKE_TYPE_TO_INT(uint8_t, int8_t);
        MAKE_TYPE_TO_INT(int16_t, int16_t);
        MAKE_TYPE_TO_INT(uint16_t, int16_t);
        MAKE_TYPE_TO_INT(int32_t, int32_t);
        MAKE_TYPE_TO_INT(uint32_t, int32_t);
        MAKE_TYPE_TO_INT(int64_t, int64_t);
        MAKE_TYPE_TO_INT(uint64_t, int64_t);
        MAKE_TYPE_TO_INT(float, int32_t);
        MAKE_TYPE_TO_INT(double, int64_t);

        template<typename T> struct type_to_primitive {};

    #define MAKE_TYPE_TO_PRIMITIVE(TYPE, PRIMITIVE)\
        template<> struct type_to_primitive<TYPE> {static constexpr spark::shared::Primitive value = spark::shared::Primitive::PRIMITIVE;};

        MAKE_TYPE_TO_PRIMITIVE(int8_t, Char);
        MAKE_TYPE_TO_PRIMITIVE(uint8_t, UChar);
        MAKE_TYPE_TO_PRIMITIVE(int16_t, Short);
        MAKE_TYPE_TO_PRIMITIVE(uint16_t, UShort);
        MAKE_TYPE_TO_PRIMITIVE(int32_t, Int);
        MAKE_TYPE_TO_PRIMITIVE(uint32_t, UInt);
        MAKE_TYPE_TO_PRIMITIVE(int64_t, Long);
        MAKE_TYPE_TO_PRIMITIVE(uint64_t, ULong);
        MAKE_TYPE_TO_PRIMITIVE(float, Float);
        MAKE_TYPE_TO_PRIMITIVE(double, Double);
    }
}