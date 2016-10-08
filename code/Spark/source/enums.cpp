// spark
#include "spark/common.h"
#include "spark/enums.h"
#include "spark/error.h"

using namespace Spark;

const char* spark_nodetype_to_str(nodetype_t val)
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
	};
	static_assert(countof(nodeNames) == NodeType::Count, "size mismatch between nodeNames and NodeType::Count");
	SPARK_ASSERT(val < NodeType::Count);

	return nodeNames[val];
}

const char* spark_control_to_str(control_t val)
{
	static const char* names[] =
	{
		"Control::Root",
		"Control::ParameterList",
		"Control::ScopeBlock",
		"Control::If",
		"Control::ElseIf",
		"Control::Else",
		"Control::While",
		"Control::For",
		"Control::Break",
	};
	SPARK_ASSERT(val < Control::Count);
	return names[val];
}

const char* spark_datatype_to_str(datatype_t val, char* buffer, int32_t sz)
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
		nullptr,
		"2",
		"3",
		"4",
		"8",
		"16",
	};

	static const char* containerNames[] =
	{
		nullptr,
		"buffer1d ",
		"buffer2d ",
		"pointer ",
	};

	auto primitive = val & DataType::PrimitiveMask;
	auto component = val & DataType::ComponentMask;
	auto container = val & DataType::ContainerMask;

	// temporary asserts until other vectors and containers are implemented
	SPARK_ASSERT(component == 0 || component == DataType::Vector2);
	SPARK_ASSERT(container == 0 || container == DataType::Pointer);

	const char* primitiveName = primitiveNames[primitive >> DataType::PrimitiveShift];
	const char* componentName = component ? componentNames[component >> DataType::ComponentShift] : "";
	const char* containerName = container ? containerNames[container >> DataType::ContainerShift ] : "";

	int32_t len = 1 + snprintf(nullptr, 0, "%s%s%s", containerName, primitiveName, componentName);
	if(len > sz)
	{
		SPARK_ASSERT(len > sz);
		return nullptr;
	}
	snprintf(buffer, sz, "%s%s%s", containerName, primitiveName, componentName);

	return buffer;
}

const char* spark_operator_to_str(operator_t val)
{
	static const char* operatorNames[] =
	{
		"Operator::Negate",
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
		"Operator::LogicalNot",
		"Operator::LogicalAnd",
		"Operator::LogicalOr",
		"Operator::BitwiseNot",
		"Operator::BitwiseAnd",
		"Operator::BitwiseOr",
		"Operator::BitwiseXor",
		"Operator::RightShift",
		"Operator::LeftShift",
        "Operator::PrefixIncrement",
        "Operator::PostfixIncrement",
        "Operator::PrefixDecrement",
        "Operator::PostfixDecrement",
		"Operator::Assignment",
		"Operator::Cast",
		"Operator::Index",
		"Operator::Property",
		"Operator::Call",
		"Operator::Return",
		"Operator::AddressOf",
		"Operator::Dereference",
	};
	static_assert(countof(operatorNames) == Operator::Count, "size mismatch between operatorNames and Operator::Count");
	SPARK_ASSERT(val < Operator::Count);

	return operatorNames[val];
}

const char* spark_property_to_str(property_t val, char* buffer, int32_t sz)
{
	if(val >= Property::FirstProperty)
	{
		const static char* propertyNames[] =
		{
			"lo",
			"hi",
			"even",
			"odd",
		};
		static_assert(countof(propertyNames) == Property::Count - Property::FirstProperty, "size mismatch between propertyNames and Property::Count");
		SPARK_ASSERT(val < Property::Count);

		snprintf(buffer, sz, "%s", propertyNames[val - Property::FirstProperty]);
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