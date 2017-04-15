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
            typedef spark::device_buffer1d<typename T::host_type> host_type;

            // extern constructor
            buffer1d(extern_construct_t)
            : _data(extern_construct)
            , Count(extern_construct)
            { }

            // move constructor
            buffer1d(buffer1d&& other)
            : _data(std::move(other._data))
            , Count(std::move(const_cast<Int&>(other.Count)))
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

            const Int Count;
        private:
            Pointer<T> _data;
        };

        template<typename T>
        struct buffer_view1d
        {
            buffer_view1d(buffer1d<T>& buffer, const rvalue<Int>& count)
            : _data(buffer.Data())
            , Count(count)
            { }

            buffer_view1d(buffer1d<T>& buffer, const rvalue<Int>& offset, const rvalue<Int>& count)
            : _data(buffer.Data() + offset)
            , Count(count)
            {

            }

            buffer_view1d(const rvalue<Pointer<T>> head, const rvalue<Int>& count)
            : _data(head)
            , Count(count)
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

            rvalue<Int> Count;
        private:
            Pointer<T> _data;
        };

        template<typename T>
        struct buffer2d
        {
            typedef spark::device_buffer2d<typename T::host_type> host_type;

            // extern constructor
            buffer2d(extern_construct_t)
            : _data(extern_construct)
            , Rows(extern_construct)
            , Columns(extern_construct)
            {

            }

            buffer2d(buffer2d&& other)
            : _data(std::move(other._data))
            , Rows(std::move(const_cast<Int&>(other.Rows)))
            , Columns(std::move(const_cast<Int&>(other.Columns)))
            {

            }

            rvalue<Pointer<T>> Data()
            {
                return rvalue<Pointer<T>>(_data._node);
            }

            const buffer_view1d<T> operator[](const rvalue<Int>& index) const
            {
                return buffer_view1d<T>(_data + (index * Columns), Columns);
            }

            buffer_view1d<T> operator[](const rvalue<Int>& index)
            {
                return buffer_view1d<T>(_data + (index * Columns), Columns);
            }

            lvalue<T> operator[](const rvalue<Int2>& index)
            {
                return _data[index.X * Columns + index.Y];
            }

            rvalue<T> operator[](const rvalue<Int2>& index) const
            {
                return _data[index.X * Columns + index.Y];
            }

            const Int Rows;
            const Int Columns;

        private:
            Pointer<T> _data;
        };
    }
    template<typename T>
    using Buffer1D = client::buffer1d<T>;
    template<typename T>
    using Buffer2D = client::buffer2d<T>;
    template<typename T>
    using BufferView1D = client::buffer_view1d<T>;
}