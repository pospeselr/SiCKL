#pragma once

namespace spark
{
    inline
	void Comment(const char* comment)
	{
		// create comment node
		auto commentNode = spark_create_comment_node(comment, SPARK_THROW_ON_ERROR());
		auto currentScope = spark_peek_scope_node(SPARK_THROW_ON_ERROR());
		spark_add_child_node(currentScope, commentNode,SPARK_THROW_ON_ERROR());
	}
}