#pragma once
namespace Spark
{
	void Comment(const char* comment)
	{
		// create comment node
		Node* commentNode = spark_create_comment_node(comment, Spark::Internal::ThrowOnError());
		Node* currentScope = spark_peek_scope_node(Spark::Internal::ThrowOnError());
		spark_add_child_node(currentScope, commentNode,Spark::Internal::ThrowOnError());
	}
}