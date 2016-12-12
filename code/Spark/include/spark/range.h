#pragma once

namespace Spark
{
	template<typename RANGE>
	struct iterator
	{
		iterator() = default;
		iterator(RANGE* range) : _range(range)
		{
			_range->init();

			Node* root = spark_create_control_node(Control::For, Spark::Internal::ThrowOnError());
			spark_add_child_node(spark_peek_scope_node(Spark::Internal::ThrowOnError()), root, Spark::Internal::ThrowOnError());
			spark_push_scope_node(root, Spark::Internal::ThrowOnError());

			Node* parameterList = spark_create_control_node(Control::ParameterList, Spark::Internal::ThrowOnError());
			spark_add_child_node(root, parameterList, Spark::Internal::ThrowOnError());

			spark_push_scope_node(parameterList, Spark::Internal::ThrowOnError());
		}

		~iterator()
		{
			if(_range)
			{
				spark_pop_scope_node(Spark::Internal::ThrowOnError());
				spark_pop_scope_node(Spark::Internal::ThrowOnError());
			}
		}

		bool operator!=(const iterator&)
		{
			if(_result)
			{
				Node* compare = spark_create_control_node(Control::ScopeBlock, Spark::Internal::ThrowOnError());
				spark_add_child_node(spark_peek_scope_node(Spark::Internal::ThrowOnError()), compare, Spark::Internal::ThrowOnError());
				spark_push_scope_node(compare, Spark::Internal::ThrowOnError());

				_range->compare();

				spark_pop_scope_node(Spark::Internal::ThrowOnError());
			}
			return _result;
		}

		iterator& operator++()
		{
			_result = false;
			return *this;
		}

		auto& operator*()
		{
			Node* update = spark_create_control_node(Control::ScopeBlock, Spark::Internal::ThrowOnError());
			spark_add_child_node(spark_peek_scope_node(Spark::Internal::ThrowOnError()), update, Spark::Internal::ThrowOnError());
			spark_push_scope_node(update, Spark::Internal::ThrowOnError());

			_range->update_value();

			spark_pop_scope_node(Spark::Internal::ThrowOnError());
			spark_pop_scope_node(Spark::Internal::ThrowOnError());

			Node* body = spark_create_control_node(Control::ScopeBlock, Spark::Internal::ThrowOnError());
			spark_add_child_node(spark_peek_scope_node(Spark::Internal::ThrowOnError()), body,Spark::Internal::ThrowOnError());
			spark_push_scope_node(body, Spark::Internal::ThrowOnError());

			return _range->get_value();
		}

	private:
		RANGE* _range = nullptr;
		bool _result = true;
	};

	template<typename TYPE>
	struct Range
	{
		Range(const rvalue<TYPE>& start, const rvalue<TYPE>& stop) : Range(start, stop, 1) {}

		Range(const rvalue<TYPE>& start, const rvalue<TYPE>& stop, const rvalue<TYPE>& step)
		: _start(start)
		, _stop(stop)
		, _step(step)
		, _val(nullptr)
		{

		}

		iterator<Range> begin()
		{
			return iterator<Range>(this);
		}

		iterator<Range> end()
		{
			return iterator<Range>();
		}

		void init()
		{
			Comment("Init");
			_val = _start;
		}

		TYPE& get_value()
		{
			return _val;
		}

		void update_value()
		{
			Comment("Update Value");
			_val = _val + _step;
		}

		void compare()
		{
			Comment("Compare");
			If(_val >= _stop)
			{
			 	Break();
			}
		}
		TYPE _start;
		TYPE _stop;
		TYPE _step;
		TYPE _val;
	};
}