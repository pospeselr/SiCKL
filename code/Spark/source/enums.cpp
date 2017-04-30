#include "spark.hpp"

#include "error.hpp"
#include "node.hpp"

using namespace spark;
using namespace spark::lib;
using namespace spark::shared;

const char* spark_nodetype_to_str(spark_nodetype_t val)
{
	static const char* nodeNames[] =
	{
		"NodeType::Control",
		"NodeType::Operator",
		"NodeType::Function",
		"NodeType::Symbol",
		"NodeType::Constant",
		"NodeType::Property",
		"NodeType::Comment",
		"NodeType::Vector",
		"NodeType::ScopeBlock",
	};
	static_assert(ruff::countof(nodeNames) == (size_t)spark_nodetype::count, "size mismatch between nodeNames and spark_nodetype::count");
	SPARK_ASSERT(val < spark_nodetype::count);

	return nodeNames[static_cast<size_t>(val)];
}

const char* spark_control_to_str(spark_control_t val)
{
	static const char* controlNames[] =
	{
		"Control::Root",
		"Control::ParameterList",
		"Control::If",
		"Control::ElseIf",
		"Control::Else",
		"Control::While",
	};
	static_assert(ruff::countof(controlNames) == static_cast<size_t>(Control::Count), "size mismatch between contorlNames and spark_control::count");
	SPARK_ASSERT(val < static_cast<size_t>(Control::Count));
	return controlNames[static_cast<size_t>(val)];
}

const char* spark_datatype_to_str(spark_datatype_t val, char* buffer, int32_t sz)
{
	static const char* primitiveNames[] =
	{
		"void",
		"char",
		"uchar",
		"short",
		"ushort",
		"int",
		"uint",
		"long",
		"ulong",
		"float",
		"double",
	};

	static const char* componentNames[] =
	{
		"",
		"",
		"2",
		"3",
		"4",
		"8",
		"16",
	};


	auto dt = static_cast<Datatype>(val);

	auto primitive = dt.GetPrimitive();
	auto components = dt.GetComponents();
	auto pointer = dt.GetPointer();

	// temporary asserts until other vectors and containers are implemented
	SPARK_ASSERT(
		components == Components::Scalar ||
		components == Components::Vector2 ||
		components == Components::Vector4 ||
		components == Components::None);

	const char* primitiveName = primitiveNames[static_cast<uint32_t>(primitive)];
	const char* componentName = componentNames[static_cast<uint32_t>(components)];
	const char* containerName = pointer ? "pointer " : "";

	int32_t len = 1 + snprintf(nullptr, 0, "%s%s%s", containerName, primitiveName, componentName);
	if(len > sz)
	{
		SPARK_ASSERT(len > sz);
		return nullptr;
	}
	snprintf(buffer, sz, "%s%s%s", containerName, primitiveName, componentName);

	return buffer;
}

const char* spark_operator_to_str(spark_operator_t val)
{
	static const char* operatorNames[] =
	{
		"Operator::Break",
		"Operator::Negate",
		"Operator::AddressOf",
		"Operator::PrefixIncrement",
		"Operator::PrefixDecrement",
		"Operator::LogicalNot",
		"Operator::BitwiseNot",
		"Operator::Dereference",
		"Operator::PostfixIncrement",
		"Operator::PostfixDecrement",
		"Operator::Add",
		"Operator::Subtract",
		"Operator::Multiply",
		"Operator::Divide",
		"Operator::Modulo",
		"Operator::GreaterThan",
		"Operator::LessThan",
		"Operator::GreaterEqualThan",
		"Operator::LessEqualThan",
		"Operator::NotEqual",
		"Operator::Equal",
		"Operator::LogicalAnd",
		"Operator::LogicalOr",
		"Operator::BitwiseAnd",
		"Operator::BitwiseOr",
		"Operator::BitwiseXor",
		"Operator::RightShift",
		"Operator::LeftShift",
		"Operator::Assignment",
		"Operator::Call",
		"Operator::Property",
		"Operator::Return",
		"Operator::Cast",
		"Operator::Index",
		"Operator::NormalizedIndex",
		"Operator::ArcCos",
		"Operator::ArcCosh",
		"Operator::ArcSin",
		"Operator::ArcSinh",
		"Operator::ArcTan",
		"Operator::ArcTan2",
		"Operator::ArcTanh",
		"Operator::CubeRoot",
		"Operator::Ceiling",
		"Operator::Cos",
		"Operator::Exp",
		"Operator::Exp2",
		"Operator::Exp10",
		"Operator::FloatAbs",
		"Operator::Floor",
		"Operator::FusedMultiplyAdd",
		"Operator::FloatMax",
		"Operator::FloatMin",
		"Operator::FloatModulus",
		"Operator::Fraction",
		"Operator::Hypotenuse",
		"Operator::LogGamma",
		"Operator::Log",
		"Operator::Log2",
		"Operator::Log10",
		"Operator::Log1Plus",
		"Operator::MultiplyAdd",
		"Operator::Pow",
		"Operator::Remainder",
		"Operator::InverseSquareRoot",
		"Operator::Sin",
		"Operator::Sinh",
		"Operator::SquareRoot",
		"Operator::Tan",
		"Operator::Tanh",
		"Operator::Gamma",
		"Operator::Truncate",
		"Operator::CrossProduct",
		"Operator::DotProduct",
		"Operator::Distance",
		"Operator::Length",
		"Operator::Normalize",
		"Operator::FastDistance",
		"Operator::FastLength",
		"Operator::FastNormalize",
		"Operator::IntegerAbs",
		"Operator::AbsDiff",
		"Operator::IntegerMax",
		"Operator::IntegerMin",
		"Operator::Clamp",
		"Operator::Degrees",
		"Operator::Radians",
		"Operator::Mix",
		"Operator::Step",
		"Operator::SmoothStep",
		"Operator::Sign",
	};
	static_assert(ruff::countof(operatorNames) == static_cast<size_t>(Operator::Count), "size mismatch between operatorNames and Operator::Count");
	SPARK_ASSERT(val < static_cast<spark_operator_t>(Operator::Count));

	return operatorNames[val];
}

const char* spark_property_to_str(spark_property_t val, char* buffer, int32_t sz)
{
	const auto property = static_cast<Property>(val);
	if(property >= Property::FirstProperty)
	{
		const static char* propertyNames[] =
		{
			"lo",
			"hi",
			"even",
			"odd",
		};

		static_assert(ruff::countof(propertyNames) == static_cast<size_t>(Property::Count - Property::FirstProperty), "size mismatch between propertyNames and Property::Count");
		SPARK_ASSERT(val < static_cast<spark_property_t>(Property::Count));

		snprintf(buffer, sz, "%s", propertyNames[static_cast<spark_property_t>(property - Property::FirstProperty)]);
	}
	else
	{
		auto getChar = [](int i) -> char
		{
			SPARK_ASSERT(i < 5);
			SPARK_ASSERT(i != 0);
			switch(i)
			{
				case 1:
				case 2:
				case 3:
					return 'x' + i - 1;
				case 4:
					return 'w';
			}
			SPARK_ASSERT(!!"Unexpected swizzle value");
			return 0;
		};

		char stack[4];
		int32_t idx = 0;
		uint32_t swiz = val;
		while(swiz > 0)
		{
			stack[idx++] = getChar(swiz % 5);
			swiz /= 5;
		}

		SPARK_ASSERT(idx <= sz);

		buffer[idx] = 0;
		for(int k = 0; k < idx; k++)
		{
			buffer[k] = stack[idx - k - 1];
		}
	}

	return buffer;
}