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
            Symbol,
            Constant,

            Count
        };
    }
    typedef enum NodeType::Type nodetype_t;

    namespace Control
    {
        enum Type
        {
            Root,
            Function,
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
            Assignment,
            Cast,

            Count
        };
    }
    typedef enum Operator::Type operator_t;

    namespace Property
    {
        enum Type
        {
            // vector components
            W,
            X,
            Y,
            Z,
            // two-component swizzles
            WW,
            WX,
            WY,
            WZ,
            XW,
            XX,
            XY,
            XZ,
            YW,
            YX,
            YY,
            YZ,
            ZW,
            ZX,
            ZY,
            ZZ,
            // three-component swizzles
            WWW,
            WWX,
            WWY,
            WWZ,
            WXW,
            WXX,
            WXY,
            WXZ,
            WYW,
            WYX,
            WYY,
            WYZ,
            WZW,
            WZX,
            WZY,
            WZZ,
            XWW,
            XWX,
            XWY,
            XWZ,
            XXW,
            XXX,
            XXY,
            XXZ,
            XYW,
            XYX,
            XYY,
            XYZ,
            XZW,
            XZX,
            XZY,
            XZZ,
            YWW,
            YWX,
            YWY,
            YWZ,
            YXW,
            YXX,
            YXY,
            YXZ,
            YYW,
            YYX,
            YYY,
            YYZ,
            YZW,
            YZX,
            YZY,
            YZZ,
            ZWW,
            ZWX,
            ZWY,
            ZWZ,
            ZXW,
            ZXX,
            ZXY,
            ZXZ,
            ZYW,
            ZYX,
            ZYY,
            ZYZ,
            ZZW,
            ZZX,
            ZZY,
            ZZZ,
            // four-component swizzles
            WWWW,
            WWWX,
            WWWY,
            WWWZ,
            WWXW,
            WWXX,
            WWXY,
            WWXZ,
            WWYW,
            WWYX,
            WWYY,
            WWYZ,
            WWZW,
            WWZX,
            WWZY,
            WWZZ,
            WXWW,
            WXWX,
            WXWY,
            WXWZ,
            WXXW,
            WXXX,
            WXXY,
            WXXZ,
            WXYW,
            WXYX,
            WXYY,
            WXYZ,
            WXZW,
            WXZX,
            WXZY,
            WXZZ,
            WYWW,
            WYWX,
            WYWY,
            WYWZ,
            WYXW,
            WYXX,
            WYXY,
            WYXZ,
            WYYW,
            WYYX,
            WYYY,
            WYYZ,
            WYZW,
            WYZX,
            WYZY,
            WYZZ,
            WZWW,
            WZWX,
            WZWY,
            WZWZ,
            WZXW,
            WZXX,
            WZXY,
            WZXZ,
            WZYW,
            WZYX,
            WZYY,
            WZYZ,
            WZZW,
            WZZX,
            WZZY,
            WZZZ,
            XWWW,
            XWWX,
            XWWY,
            XWWZ,
            XWXW,
            XWXX,
            XWXY,
            XWXZ,
            XWYW,
            XWYX,
            XWYY,
            XWYZ,
            XWZW,
            XWZX,
            XWZY,
            XWZZ,
            XXWW,
            XXWX,
            XXWY,
            XXWZ,
            XXXW,
            XXXX,
            XXXY,
            XXXZ,
            XXYW,
            XXYX,
            XXYY,
            XXYZ,
            XXZW,
            XXZX,
            XXZY,
            XXZZ,
            XYWW,
            XYWX,
            XYWY,
            XYWZ,
            XYXW,
            XYXX,
            XYXY,
            XYXZ,
            XYYW,
            XYYX,
            XYYY,
            XYYZ,
            XYZW,
            XYZX,
            XYZY,
            XYZZ,
            XZWW,
            XZWX,
            XZWY,
            XZWZ,
            XZXW,
            XZXX,
            XZXY,
            XZXZ,
            XZYW,
            XZYX,
            XZYY,
            XZYZ,
            XZZW,
            XZZX,
            XZZY,
            XZZZ,
            YWWW,
            YWWX,
            YWWY,
            YWWZ,
            YWXW,
            YWXX,
            YWXY,
            YWXZ,
            YWYW,
            YWYX,
            YWYY,
            YWYZ,
            YWZW,
            YWZX,
            YWZY,
            YWZZ,
            YXWW,
            YXWX,
            YXWY,
            YXWZ,
            YXXW,
            YXXX,
            YXXY,
            YXXZ,
            YXYW,
            YXYX,
            YXYY,
            YXYZ,
            YXZW,
            YXZX,
            YXZY,
            YXZZ,
            YYWW,
            YYWX,
            YYWY,
            YYWZ,
            YYXW,
            YYXX,
            YYXY,
            YYXZ,
            YYYW,
            YYYX,
            YYYY,
            YYYZ,
            YYZW,
            YYZX,
            YYZY,
            YYZZ,
            YZWW,
            YZWX,
            YZWY,
            YZWZ,
            YZXW,
            YZXX,
            YZXY,
            YZXZ,
            YZYW,
            YZYX,
            YZYY,
            YZYZ,
            YZZW,
            YZZX,
            YZZY,
            YZZZ,
            ZWWW,
            ZWWX,
            ZWWY,
            ZWWZ,
            ZWXW,
            ZWXX,
            ZWXY,
            ZWXZ,
            ZWYW,
            ZWYX,
            ZWYY,
            ZWYZ,
            ZWZW,
            ZWZX,
            ZWZY,
            ZWZZ,
            ZXWW,
            ZXWX,
            ZXWY,
            ZXWZ,
            ZXXW,
            ZXXX,
            ZXXY,
            ZXXZ,
            ZXYW,
            ZXYX,
            ZXYY,
            ZXYZ,
            ZXZW,
            ZXZX,
            ZXZY,
            ZXZZ,
            ZYWW,
            ZYWX,
            ZYWY,
            ZYWZ,
            ZYXW,
            ZYXX,
            ZYXY,
            ZYXZ,
            ZYYW,
            ZYYX,
            ZYYY,
            ZYYZ,
            ZYZW,
            ZYZX,
            ZYZY,
            ZYZZ,
            ZZWW,
            ZZWX,
            ZZWY,
            ZZWZ,
            ZZXW,
            ZZXX,
            ZZXY,
            ZZXZ,
            ZZYW,
            ZZYX,
            ZZYY,
            ZZYZ,
            ZZZW,
            ZZZX,
            ZZZY,
            ZZZZ,
            // Other vector properties
            Lo,
            Hi,
            Even,
            Odd,
        };
    }
    typedef Property::Type property_t;
}

extern "C" const char* spark_nodetype_to_str(Spark::nodetype_t);
extern "C" const char* spark_control_to_str(Spark::control_t);
extern "C" const char* spark_datatype_to_str(Spark::datatype_t, char* buffer, int32_t sz);
extern "C" const char* spark_operator_to_str(Spark::operator_t);
