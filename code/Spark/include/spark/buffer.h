#pragma once

template<typename T>
struct get_typename;

namespace spark
{
    namespace client
    {
        template<typename T>
        struct buffer1d
        {
        public:
            typedef spark::device_buffer1d<typename T::host_type> host_type;

            // extern constructor
            buffer1d(extern_construct_t)
            : _data(extern_construct)
            , Size(extern_construct)
            {

            }

            // move constructor
            buffer1d(buffer1d&& other)
            : _data(std::move(other._data))
            , Size(std::move(const_cast<Int&>(other.Size)))
            {

            }

            lvalue<T> operator[](const rvalue<Int>& index)
            {
                return _data[index];
            }

            rvalue<T> operator[](const rvalue<Int>& index) const
            {
                return _data[index];
            }

            rvalue<Pointer<T>> Data()
            {
                return rvalue<Pointer<T>>(_data._node);
            }

            const Int Size;
        private:
            Pointer<T> _data;
        };
    }
    template<typename T>
    using Buffer1D = client::buffer1d<T>;
}