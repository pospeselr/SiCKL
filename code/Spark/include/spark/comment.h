#pragma once

namespace spark
{
	void Comment(const char* comment)
	{
		// create comment node
		auto commentNode = spark_create_comment_node(comment, THROW_ON_ERROR());
		auto currentScope = spark_peek_scope_node(THROW_ON_ERROR());
		spark_add_child_node(currentScope, commentNode,THROW_ON_ERROR());
	}
}