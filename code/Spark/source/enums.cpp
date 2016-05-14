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

const char* spark_function_to_str(function_t val)
{
	SPARK_ASSERT(val < Function::Count);

	static const char* functionNames[] =
	{
		"Function::Negate",
		"Function::Add",
		"Function::Subtract",
		"Function::Multiply",
		"Function::Divide",
		"Function::Modulo",
		"Function::GreaterThan",
		"Function::LessThan",
		"Function::GreaterEqualThan",
		"Function::LessEqualThan",
		"Function::NotEqual",
		"Function::Equal",
		"Function::LogicalNot",
		"Function::LogicalAnd",
		"Function::LogicalOr",
		"Function::BitwiseNot",
		"Function::BitwiseAnd",
		"Function::BitwiseOr",
		"Function::BitwiseXor",
		"Function::RightShift",
		"Function::LeftShift",
		"Function::Assignment",
		"Function::Cast",
	};

	return functionNames[val];
}