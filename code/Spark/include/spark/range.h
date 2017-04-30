#pragma once

namespace spark
{
	template<typename RANGE>
	struct iterator
	{
		iterator() = default;
		iterator(RANGE* range) : _range(range)
		{
			auto scope = spark_create_scope_block_node(SPARK_THROW_ON_ERROR());
			spark_add_child_node(spark_peek_scope_node(SPARK_THROW_ON_ERROR()), scope, SPARK_THROW_ON_ERROR());
			spark_push_scope_node(scope, SPARK_THROW_ON_ERROR());

			_range->init();

			auto while_node = spark_create_control_node(static_cast<spark_control_t>(spark::shared::Control::While), SPARK_THROW_ON_ERROR());
			spark_add_child_node(scope, while_node, SPARK_THROW_ON_ERROR());
			spark_push_scope_node(while_node, SPARK_THROW_ON_ERROR());

			int32_t true_val = 1;
			auto true_node = spark_create_constant_node(static_cast<spark_datatype_t>(Int::type), &true_val, sizeof(true_val), SPARK_THROW_ON_ERROR());
			spark_add_child_node(while_node, true_node, SPARK_THROW_ON_ERROR());

			auto body_scope = spark_create_scope_block_node(SPARK_THROW_ON_ERROR());
			spark_add_child_node(while_node, body_scope, SPARK_THROW_ON_ERROR());
			spark_push_scope_node(body_scope, SPARK_THROW_ON_ERROR());
		}

		~iterator()
		{
			if(_range)
			{
				_range->update_value();
				spark_pop_scope_node(SPARK_THROW_ON_ERROR());
				spark_pop_scope_node(SPARK_THROW_ON_ERROR());
				spark_pop_scope_node(SPARK_THROW_ON_ERROR());
			}

		}

		bool operator!=(const iterator&)
		{
			if(_result)
			{
				_range->compare();
			}
			return _result;
		}

		iterator& operator++()
		{
			_result = false;
			return *this;
		}

		auto operator*()
		{
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
		: _temp_start(start._node)
		, _temp_stop(stop._node)
		, _temp_step(step._node)
		, _val(spark::client::null_construct)
		, _stop(spark::client::null_construct)
		, _step(spark::client::null_construct)
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
			Int start = _temp_start;
			Int stop = _temp_stop;
			Int step = _temp_step;

			_val = std::move(start);
			_stop = std::move(stop);
			_step = std::move(step);
		}

		const TYPE get_value()
		{
			return std::move(_val);
		}

		void update_value()
		{
			_val = _val + _step;
		}

		void compare()
		{
			If(_val >= _stop)
			{
			 	Break();
			}
		}
		spark::client::rvalue<TYPE> _temp_start;
		spark::client::rvalue<TYPE> _temp_stop;
		spark::client::rvalue<TYPE> _temp_step;
		TYPE _val;
		TYPE _stop;
		TYPE _step;
	};
}