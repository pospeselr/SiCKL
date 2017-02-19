#pragma once

namespace spark
{
    namespace lib
    {
        template<typename element_type, typename deleter_type, deleter_type deleter>
        class unique_any
        {
        public:
            unique_any() {}
            unique_any(element_type val) : _val(val), _empty(false) {}
            unique_any(unique_any&& that) : _val(that.val), _empty(that.empty)
            {
                that.val = {};
                that._empty = true;
            }
            unique_any(unique_any&) = delete;

            unique_any& operator=(unique_any&& that)
            {
                this->reset(that._val);

                that._val = {};
                that._empty = true;

                return *this;
            }

            ~unique_any()
            {
                this->reset();
            }

            element_type release()
            {
                element_type result = _val;
                _val = {};
                _empty = true;

                return result;
            }

            void reset(element_type val)
            {
                if(!_empty)
                {
                    deleter(_val);
                }
                else
                {
                    _empty = true;
                }
                _val = val;
            }

            void reset()
            {
                if(!_empty)
                {
                    deleter(_val);
                    _val = {};
                    _empty = true;
                }
            }

            void swap(unique_any& other)
            {
                element_type temp = this->_val;
                this->_val = other._val;
                other._val = temp;
            }

            element_type get()
            {
                return _val;
            }

            operator bool() const
            {
                return !_empty;
            }

        private:
            element_type _val = {};
            bool _empty = true;
        };
    }
}