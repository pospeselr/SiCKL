#pragma once

namespace spark
{
	template<typename RANGE>
	struct iterator
	{
		iterator() = default;
		iterator(RANGE* range) : _range(range)
		{
			_range->init();

			auto root = spark_create_control_node(static_cast<spark_control_t>(spark::shared::Control::For), SPARK_THROW_ON_ERROR());
			spark_add_child_node(spark_peek_scope_node(SPARK_THROW_ON_ERROR()), root, SPARK_THROW_ON_ERROR());
			spark_push_scope_node(root, SPARK_THROW_ON_ERROR());

			auto parameterList = spark_create_control_node(static_cast<spark_control_t>(spark::shared::Control::ParameterList), SPARK_THROW_ON_ERROR());
			spark_add_child_node(root, parameterList, SPARK_THROW_ON_ERROR());

			spark_push_scope_node(parameterList, SPARK_THROW_ON_ERROR());
		}

		~iterator()
		{
			if(_range)
			{
				spark_pop_scope_node(SPARK_THROW_ON_ERROR());
				spark_pop_scope_node(SPARK_THROW_ON_ERROR());
			}
		}

		bool operator!=(const iterator&)
		{
			if(_result)
			{
				auto compare = spark_create_scope_block_node(SPARK_THROW_ON_ERROR());
				spark_add_child_node(spark_peek_scope_node(SPARK_THROW_ON_ERROR()), compare, SPARK_THROW_ON_ERROR());
				spark_push_scope_node(compare, SPARK_THROW_ON_ERROR());

				_range->compare();

				spark_pop_scope_node(SPARK_THROW_ON_ERROR());
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
			auto update = spark_create_scope_block_node(SPARK_THROW_ON_ERROR());
			spark_add_child_node(spark_peek_scope_node(SPARK_THROW_ON_ERROR()), update, SPARK_THROW_ON_ERROR());
			spark_push_scope_node(update, SPARK_THROW_ON_ERROR());

			_range->update_value();

			spark_pop_scope_node(SPARK_THROW_ON_ERROR());
			spark_pop_scope_node(SPARK_THROW_ON_ERROR());

			auto body = spark_create_scope_block_node(SPARK_THROW_ON_ERROR());
			spark_add_child_node(spark_peek_scope_node(SPARK_THROW_ON_ERROR()), body, SPARK_THROW_ON_ERROR());
			spark_push_scope_node(body, SPARK_THROW_ON_ERROR());

			return _range->get_value();
		}

	private:
		RANGE* _range = nullptr;
		bool _result = true;
	};

	template<typename TYPE>
	struct Range
	{
		Range(const spark::client::rvalue<TYPE>& start, const spark::client::rvalue<TYPE>& stop) : Range(start, stop, 1) {}

		Range(const spark::client::rvalue<TYPE>& start, const spark::client::rvalue<TYPE>& stop, const spark::client::rvalue<TYPE>& step)
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