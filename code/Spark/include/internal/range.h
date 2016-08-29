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

			Node* root = spark_create_control_node(Control::For);
			spark_add_child_node(spark_peek_scope_node(), root);
			spark_push_scope_node(root);

			Node* parameterList = spark_create_control_node(Control::ParameterList);
			spark_add_child_node(root, parameterList);

			spark_push_scope_node(parameterList);
		}

		~iterator()
		{
			if(_range)
			{
				spark_pop_scope_node();
				spark_pop_scope_node();
			}
		}

		bool operator!=(const iterator&)
		{
			if(_result)
			{
				Node* compare = spark_create_control_node(Control::ScopeBlock);
				spark_add_child_node(spark_peek_scope_node(), compare);
				spark_push_scope_node(compare);

				_range->compare();

				spark_pop_scope_node();
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
			Node* update = spark_create_control_node(Control::ScopeBlock);
			spark_add_child_node(spark_peek_scope_node(), update);
			spark_push_scope_node(update);

			_range->update_value();

			spark_pop_scope_node();
			spark_pop_scope_node();

			Node* body = spark_create_control_node(Control::ScopeBlock);
			spark_add_child_node(spark_peek_scope_node(), body);
			spark_push_scope_node(body);

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
#if 0
	struct Range
	{
		struct RangeIterator
		{
			bool operator!=(const RangeIterator&)
			{
				TRACE
				return _result;
			}

			RangeIterator& operator++()
			{
				_result = false;
				return *this;
			}

			Int operator*()
			{
				TRACE
				return Int();
			}

			bool _result = true;
		};

		Range(int start, int stop)
		{
			TRACE
		}

		RangeIterator start()
		{
			TRACE
			return RangeIterator();
		}

		RangeIterator end()
		{
			TRACE
			return RangeIterator();
		}
	};
#endif
}