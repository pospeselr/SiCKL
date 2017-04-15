#pragma once

typedef uint32_t spark_control_t;
typedef uint32_t spark_datatype_t;
typedef uint32_t spark_operator_t;
typedef uint32_t spark_property_t;

namespace spark
{
    namespace shared
    {
        // Helper Functions
        template<typename T>
        constexpr size_t LogBase2(T val)
        {
            size_t result = 0;
            while(val >>= 1)
            {
                result++;
            }
            return result;
        }

        template<typename T>
        constexpr size_t EnumRequiredBits(T val)
        {
            return 1 + LogBase2(val);
        }

        constexpr uint32_t Mask(size_t bits)
        {
            uint32_t result = 0;
            for(size_t k = 0; k < bits; k++)
            {
                result = (result << 1) | 1;
            }
            return result;
        }

        // Enums

        enum class Control : uint32_t
        {
            Root,
            ParameterList,
            If,
            ElseIf,
            Else,
            While,
            For,

            Count
        };

        enum class Primitive : uint32_t
        {
            Void,
            Char,
            UChar,
            Short,
            UShort,
            Int,
            UInt,
            Long,
            ULong,
            Float,
            Double,

            Count
        };

        enum class Components : uint32_t
        {
            None,
            Scalar,
            Vector2,
            Vector4,
            Vector8,
            Vector16,

            Count
        };

        class Datatype
        {
        private:
            constexpr static size_t primitive_bits = EnumRequiredBits(static_cast<spark_datatype_t>(Primitive::Count) - 1u);
            constexpr static size_t components_bits = EnumRequiredBits(static_cast<spark_datatype_t>(Components::Count) - 1u);
            constexpr static size_t pointer_bits = 1;

            constexpr static size_t primitive_offset = 0;
            constexpr static size_t components_offset = primitive_offset + primitive_bits;
            constexpr static size_t pointer_offset = components_offset + components_bits;

            constexpr static spark_datatype_t primitive_mask = Mask(primitive_bits);
            constexpr static spark_datatype_t components_mask = Mask(components_bits);
            constexpr static spark_datatype_t pointer_mask = Mask(pointer_bits);

            spark_datatype_t _primitive  : primitive_bits;
            spark_datatype_t _components : components_bits;
            spark_datatype_t _pointer    : pointer_bits;
        public:
            constexpr Datatype()
            : _primitive(0)
            , _components(0)
            , _pointer(0)
            { }
            constexpr Datatype(Primitive primitive, Components components, bool pointer)
            : _primitive(static_cast<spark_datatype_t>(primitive))
            , _components(static_cast<spark_datatype_t>(components))
            , _pointer(pointer ? 1 : 0)
            { }
            constexpr Datatype(spark_datatype_t raw)
            : _primitive((raw >> primitive_offset) & primitive_mask)
            , _components((raw >> components_offset) & components_mask)
            , _pointer((raw >> pointer_offset) & pointer_mask)
            { }

            explicit constexpr operator spark_datatype_t() const
            {
                return (_primitive) | (_components << components_offset) | (_pointer << pointer_offset);
            }

            constexpr Primitive GetPrimitive() const
            {
                return static_cast<Primitive>(_primitive);
            }

            constexpr Components GetComponents() const
            {
                return static_cast<Components>(_components);
            }

            constexpr bool GetPointer() const
            {
                return _pointer ? true : false;
            }
        };

        enum class Operator : spark_operator_t
        {
            // nullary operators
            Break,
            // prefix unary operators
            Negate,
            AddressOf,
            PrefixIncrement,
            PrefixDecrement,
            LogicalNot,
            BitwiseNot,
            Dereference,
            // postfix uniary operators
            PostfixIncrement,
            PostfixDecrement,
            // binary infix operators
            Add,
            Subtract,
            Multiply,
            Divide,
            Modulo,
            GreaterThan,
            LessThan,
            GreaterEqualThan,
            LessEqualThan,
            NotEqual,
            Equal,
            LogicalAnd,
            LogicalOr,
            BitwiseAnd,
            BitwiseOr,
            BitwiseXor,
            RightShift,
            LeftShift,
            Assignment,
            // n-ary operators
            Call,
            Property,
            Return,
            Cast,
            // built-in functions
            Index,
            NormalizedIndex,

            Count
        };

        inline constexpr Operator operator-(const Operator left, const Operator right)
        {
            return static_cast<Operator>(static_cast<spark_operator_t>(left) - static_cast<spark_operator_t>(right));
        }

        enum class Property : spark_property_t
        {
            // swizzles values are in base 5 (x == 1, w == 4)
            X = 1,
            Y = 2,
            Z = 3,
            W = 4,
            XX = 6,
            XY = 7,
            XZ = 8,
            XW = 9,
            YX = 11,
            YY = 12,
            YZ = 13,
            YW = 14,
            ZX = 16,
            ZY = 17,
            ZZ = 18,
            ZW = 19,
            WX = 21,
            WY = 22,
            WZ = 23,
            WW = 24,
            XXX = 31,
            XXY = 32,
            XXZ = 33,
            XXW = 34,
            XYX = 36,
            XYY = 37,
            XYZ = 38,
            XYW = 39,
            XZX = 41,
            XZY = 42,
            XZZ = 43,
            XZW = 44,
            XWX = 46,
            XWY = 47,
            XWZ = 48,
            XWW = 49,
            YXX = 56,
            YXY = 57,
            YXZ = 58,
            YXW = 59,
            YYX = 61,
            YYY = 62,
            YYZ = 63,
            YYW = 64,
            YZX = 66,
            YZY = 67,
            YZZ = 68,
            YZW = 69,
            YWX = 71,
            YWY = 72,
            YWZ = 73,
            YWW = 74,
            ZXX = 81,
            ZXY = 82,
            ZXZ = 83,
            ZXW = 84,
            ZYX = 86,
            ZYY = 87,
            ZYZ = 88,
            ZYW = 89,
            ZZX = 91,
            ZZY = 92,
            ZZZ = 93,
            ZZW = 94,
            ZWX = 96,
            ZWY = 97,
            ZWZ = 98,
            ZWW = 99,
            WXX = 106,
            WXY = 107,
            WXZ = 108,
            WXW = 109,
            WYX = 111,
            WYY = 112,
            WYZ = 113,
            WYW = 114,
            WZX = 116,
            WZY = 117,
            WZZ = 118,
            WZW = 119,
            WWX = 121,
            WWY = 122,
            WWZ = 123,
            WWW = 124,
            XXXX = 156,
            XXXY = 157,
            XXXZ = 158,
            XXXW = 159,
            XXYX = 161,
            XXYY = 162,
            XXYZ = 163,
            XXYW = 164,
            XXZX = 166,
            XXZY = 167,
            XXZZ = 168,
            XXZW = 169,
            XXWX = 171,
            XXWY = 172,
            XXWZ = 173,
            XXWW = 174,
            XYXX = 181,
            XYXY = 182,
            XYXZ = 183,
            XYXW = 184,
            XYYX = 186,
            XYYY = 187,
            XYYZ = 188,
            XYYW = 189,
            XYZX = 191,
            XYZY = 192,
            XYZZ = 193,
            XYZW = 194,
            XYWX = 196,
            XYWY = 197,
            XYWZ = 198,
            XYWW = 199,
            XZXX = 206,
            XZXY = 207,
            XZXZ = 208,
            XZXW = 209,
            XZYX = 211,
            XZYY = 212,
            XZYZ = 213,
            XZYW = 214,
            XZZX = 216,
            XZZY = 217,
            XZZZ = 218,
            XZZW = 219,
            XZWX = 221,
            XZWY = 222,
            XZWZ = 223,
            XZWW = 224,
            XWXX = 231,
            XWXY = 232,
            XWXZ = 233,
            XWXW = 234,
            XWYX = 236,
            XWYY = 237,
            XWYZ = 238,
            XWYW = 239,
            XWZX = 241,
            XWZY = 242,
            XWZZ = 243,
            XWZW = 244,
            XWWX = 246,
            XWWY = 247,
            XWWZ = 248,
            XWWW = 249,
            YXXX = 281,
            YXXY = 282,
            YXXZ = 283,
            YXXW = 284,
            YXYX = 286,
            YXYY = 287,
            YXYZ = 288,
            YXYW = 289,
            YXZX = 291,
            YXZY = 292,
            YXZZ = 293,
            YXZW = 294,
            YXWX = 296,
            YXWY = 297,
            YXWZ = 298,
            YXWW = 299,
            YYXX = 306,
            YYXY = 307,
            YYXZ = 308,
            YYXW = 309,
            YYYX = 311,
            YYYY = 312,
            YYYZ = 313,
            YYYW = 314,
            YYZX = 316,
            YYZY = 317,
            YYZZ = 318,
            YYZW = 319,
            YYWX = 321,
            YYWY = 322,
            YYWZ = 323,
            YYWW = 324,
            YZXX = 331,
            YZXY = 332,
            YZXZ = 333,
            YZXW = 334,
            YZYX = 336,
            YZYY = 337,
            YZYZ = 338,
            YZYW = 339,
            YZZX = 341,
            YZZY = 342,
            YZZZ = 343,
            YZZW = 344,
            YZWX = 346,
            YZWY = 347,
            YZWZ = 348,
            YZWW = 349,
            YWXX = 356,
            YWXY = 357,
            YWXZ = 358,
            YWXW = 359,
            YWYX = 361,
            YWYY = 362,
            YWYZ = 363,
            YWYW = 364,
            YWZX = 366,
            YWZY = 367,
            YWZZ = 368,
            YWZW = 369,
            YWWX = 371,
            YWWY = 372,
            YWWZ = 373,
            YWWW = 374,
            ZXXX = 406,
            ZXXY = 407,
            ZXXZ = 408,
            ZXXW = 409,
            ZXYX = 411,
            ZXYY = 412,
            ZXYZ = 413,
            ZXYW = 414,
            ZXZX = 416,
            ZXZY = 417,
            ZXZZ = 418,
            ZXZW = 419,
            ZXWX = 421,
            ZXWY = 422,
            ZXWZ = 423,
            ZXWW = 424,
            ZYXX = 431,
            ZYXY = 432,
            ZYXZ = 433,
            ZYXW = 434,
            ZYYX = 436,
            ZYYY = 437,
            ZYYZ = 438,
            ZYYW = 439,
            ZYZX = 441,
            ZYZY = 442,
            ZYZZ = 443,
            ZYZW = 444,
            ZYWX = 446,
            ZYWY = 447,
            ZYWZ = 448,
            ZYWW = 449,
            ZZXX = 456,
            ZZXY = 457,
            ZZXZ = 458,
            ZZXW = 459,
            ZZYX = 461,
            ZZYY = 462,
            ZZYZ = 463,
            ZZYW = 464,
            ZZZX = 466,
            ZZZY = 467,
            ZZZZ = 468,
            ZZZW = 469,
            ZZWX = 471,
            ZZWY = 472,
            ZZWZ = 473,
            ZZWW = 474,
            ZWXX = 481,
            ZWXY = 482,
            ZWXZ = 483,
            ZWXW = 484,
            ZWYX = 486,
            ZWYY = 487,
            ZWYZ = 488,
            ZWYW = 489,
            ZWZX = 491,
            ZWZY = 492,
            ZWZZ = 493,
            ZWZW = 494,
            ZWWX = 496,
            ZWWY = 497,
            ZWWZ = 498,
            ZWWW = 499,
            WXXX = 531,
            WXXY = 532,
            WXXZ = 533,
            WXXW = 534,
            WXYX = 536,
            WXYY = 537,
            WXYZ = 538,
            WXYW = 539,
            WXZX = 541,
            WXZY = 542,
            WXZZ = 543,
            WXZW = 544,
            WXWX = 546,
            WXWY = 547,
            WXWZ = 548,
            WXWW = 549,
            WYXX = 556,
            WYXY = 557,
            WYXZ = 558,
            WYXW = 559,
            WYYX = 561,
            WYYY = 562,
            WYYZ = 563,
            WYYW = 564,
            WYZX = 566,
            WYZY = 567,
            WYZZ = 568,
            WYZW = 569,
            WYWX = 571,
            WYWY = 572,
            WYWZ = 573,
            WYWW = 574,
            WZXX = 581,
            WZXY = 582,
            WZXZ = 583,
            WZXW = 584,
            WZYX = 586,
            WZYY = 587,
            WZYZ = 588,
            WZYW = 589,
            WZZX = 591,
            WZZY = 592,
            WZZZ = 593,
            WZZW = 594,
            WZWX = 596,
            WZWY = 597,
            WZWZ = 598,
            WZWW = 599,
            WWXX = 606,
            WWXY = 607,
            WWXZ = 608,
            WWXW = 609,
            WWYX = 611,
            WWYY = 612,
            WWYZ = 613,
            WWYW = 614,
            WWZX = 616,
            WWZY = 617,
            WWZZ = 618,
            WWZW = 619,
            WWWX = 621,
            WWWY = 622,
            WWWZ = 623,
            WWWW = 624,

            FirstProperty = WWWW + 1,

            // other vector properties
            Lo = FirstProperty,
            Hi,
            Even,
            Odd,

            Count
        };

        inline constexpr Property operator-(const Property left, const Property right)
        {
            return static_cast<Property>(static_cast<spark_property_t>(left) - static_cast<spark_property_t>(right));
        }
    }
}

//extern "C" const char* spark_nodetype_to_str(Spark::nodetype_t);
extern "C" const char* spark_control_to_str(spark_control_t val);
extern "C" const char* spark_datatype_to_str(spark_datatype_t dt, char* buffer, int32_t sz);
extern "C" const char* spark_operator_to_str(spark_operator_t op);
extern "C" const char* spark_property_to_str(spark_property_t, char* buffer, int32_t sz);
