#pragma once

namespace Spark
{
	template<typename PARENT>
	struct iterator
	{
		iterator() = default;
		iterator(PARENT* parent) : _parent(parent)
		{
			parent->init();

			Node* root = spark_create_control_node(Control::For);
			spark_add_child_node(spark_peek_scope_node(), root);
			spark_push_scope_node(root);

			Node* parameterList = spark_create_control_node(Control::ParameterList);
			spark_add_child_node(root, parameterList);

			spark_push_scope_node(parameterList);
		}

		~iterator()
		{
			TRACE
			if(_parent)
			{
				spark_pop_scope_node();
			}
		}

		bool operator!=(const iterator&)
		{
			if(_result)
			{
				TRACE
				Node* compare = spark_create_control_node(Control::ScopeBlock);
				spark_add_child_node(spark_peek_scope_node(), compare);
				spark_push_scope_node(compare);

				_parent->compare();

				spark_pop_scope_node();
			}
			else
			{
				TRACE

			}
			return _result;
		}

		iterator& operator++()
		{
			TRACE
			_result = false;
			return *this;
		}

		auto& operator*()
		{
			TRACE
			Node* update = spark_create_control_node(Control::ScopeBlock);
			spark_add_child_node(spark_peek_scope_node(), update);
			spark_push_scope_node(update);

			_parent->update_value();

			spark_pop_scope_node();

			Node* body = spark_create_control_node(Control::ScopeBlock);
			spark_add_child_node(spark_peek_scope_node(), body);
			spark_push_scope_node(body);

			return _parent->get_value();
		}

	private:
		PARENT* _parent = nullptr;
		bool _result = true;
	};

	struct Range
	{
		Range(int start, int stop)
		: _start(start),
		  _stop(stop),
		  _step(1),
		  _val(nullptr)
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

		Int& get_value()
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
		int _start;
		int _stop;
		int _step;
		Int _val;
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