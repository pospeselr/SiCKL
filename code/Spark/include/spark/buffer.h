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
            // extern constructor
            buffer1d(extern_construct_t)
            : _data(extern_construct)
            , Size(extern_construct)
            {

            }

            // move constructor
            buffer1d(buffer1d&& other)
            : _data(std::move(other._data))
            , Size(std::move(const_cast<Long&>(other.Size)))
            {

            }

            lvalue<T> operator[](const rvalue<Long>& index)
            {
                return _data[index];
            }

            rvalue<T> operator[](const rvalue<Long>& index) const
            {
                return _data[index];
            }

            rvalue<Pointer<T>> Data()
            {
                return rvalue<Pointer<T>>(_data._node);
            }

            const Long Size;
        private:
            Pointer<T> _data;
        };
    }
    template<typename T>
    using Buffer1D = client::buffer1d<T>;
}