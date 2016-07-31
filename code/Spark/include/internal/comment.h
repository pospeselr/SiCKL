#pragma once
namespace Spark
{
	void Comment(const char* comment)
	{
		// create comment node
		Node* commentNode = spark_create_comment_node(comment);
		Node* currentScope = spark_peek_scope_node();
		spark_add_child_node(currentScope, commentNode);
	}
}