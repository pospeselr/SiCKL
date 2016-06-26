#pragma once

namespace Spark
{
    typedef uint32_t symbolid_t;

    namespace NodeType
    {
        enum Type
        {
            Control,
            Operator,
            Function,
            Symbol,
            Constant,
            Property,
            Comment,

            Count
        };
    }
    typedef enum NodeType::Type nodetype_t;

    namespace Control
    {
        enum Type
        {
            Root,
            ParameterList,
            ScopeBlock,
            If,
            ElseIf,
            Else,
            While,
            ForInRange,

            Count
        };
    }
    typedef enum Control::Type control_t;

    namespace DataType
    {
        enum Type
        {
            Invalid = -1,
            // primitive
            FirstPrimitive = 0,
            Void = FirstPrimitive,
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
            PrimitiveMask = 0xF,

            // components
            FirstComponent = PrimitiveMask + 1,
            Vector2 = FirstComponent,
            Vector3,
            Vector4,
            Vector8,
            Vector16,
            ComponentMask = 0x70,

            // container
            FirstContainer = ComponentMask + 1,
            Buffer1D = FirstContainer,
            Buffer2D,
            ContainerMask = 0x180,
        };
    }
    typedef enum DataType::Type datatype_t;

    namespace Operator
    {
        enum Type
        {
            Negate,
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
            LogicalNot,
            LogicalAnd,
            LogicalOr,
            BitwiseNot,
            BitwiseAnd,
            BitwiseOr,
            BitwiseXor,
            RightShift,
            LeftShift,
            PrefixIncrement,
            PostfixIncrement,
            PrefixDecrement,
            PostfixDecrement,
            Assignment,
            Cast,
            Index,
            Property,
            Call,

            Count
        };
    }
    typedef enum Operator::Type operator_t;

    namespace Property
    {
        enum Type
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
    }
    typedef Property::Type property_t;
}

extern "C" const char* spark_nodetype_to_str(Spark::nodetype_t);
extern "C" const char* spark_control_to_str(Spark::control_t);
extern "C" const char* spark_datatype_to_str(Spark::datatype_t, char* buffer, int32_t sz);
extern "C" const char* spark_operator_to_str(Spark::operator_t);
extern "C" const char* spark_property_to_str(Spark::property_t, char* buffer, int32_t sz);
