#include "precomp.h"

#include "internal/common.h"
#include "internal/enums.h"
#include "internal/error.h"

using namespace Spark;

const char* spark_nodetype_to_str(nodetype_t val)
{
	static const char* names[] =
	{
		"NodeType::Control",
		"NodeType::Function",
		"NodeType::Symbol",
		"NodeType::Constant",
	};
	SPARK_ASSERT(val < NodeType::Count);
	return names[val];
}

const char* spark_control_to_str(control_t val)
{
	static const char* names[] =
	{
		"Control::Root",
		"Control::Function",
		"Control::ParameterList",
		"Control::ScopeBlock",
		"Control::If",
		"Control::ElseIf",
		"Control::Else",
		"Control::While",
		"Control::ForInRange",
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
		"2",
		"3",
		"4",
		"8",
		"16",
	};

	static const char* containerNames[] =
	{
		"buffer1d ",
		"buffer2d ",
	};

	auto primitive = val & DataType::PrimitiveMask;
	auto component = val & DataType::ComponentMask;
	auto container = val & DataType::ContainerMask;

	// temporary asserts until other vectors and containers are implemented
	SPARK_ASSERT(component == 0 || component == DataType::Vector2);
	SPARK_ASSERT(container == 0);

	const char* primitiveName = primitiveNames[primitive - DataType::FirstPrimitive];
	const char* componentName = component ? componentNames[component - DataType::FirstComponent] : "";
	const char* containerName = container ? containerNames[container - DataType::FirstContainer] : "";

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
	SPARK_ASSERT(val < Operator::Count);

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
		"Operator::Assignment",
		"Operator::Cast",
	};
	static_assert(countof(operatorNames) == Operator::Count, "size mismatch between operatorNames and Operator::Count");

	return operatorNames[val];
}