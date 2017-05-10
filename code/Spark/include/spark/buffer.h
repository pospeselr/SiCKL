#pragma once

template<typename T>
struct get_typename;

namespace spark
{
    namespace client
    {
        template<typename T>
        struct buffer_view1d
        {
            typedef spark::device_buffer1d<typename T::host_type> host_type;

            // extern constuctor
            buffer_view1d(extern_construct_t)
            : Count(extern_construct)
            , _stride(constant_constructor<int32_t>(Int::type, 1))
            , _data(extern_construct)
            { }

            buffer_view1d(buffer_view1d&& other)
            : Count(std::move(const_cast<Int&>(other.Count)))
            , _stride(std::move(const_cast<Int&>(other._stride)))
            , _data(std::move(other._data))
            { }

            buffer_view1d(buffer_view1d& buffer, const rvalue<Int>& count)
            : Count(count)
            , _stride(buffer._stride)
            , _data(buffer._data)
            { }

            buffer_view1d(buffer_view1d& buffer, const rvalue<Int>& count, const rvalue<Int>& stride)
            : Count(count)
            , _stride(buffer._stride * stride)
            , _data(buffer._data)
            { }

            buffer_view1d(buffer_view1d& buffer, const rvalue<Int>& count, const rvalue<Int>& stride, const rvalue<Int>& offset)
            : Count(count)
            , _stride(buffer._stride * stride)
            , _data(buffer._data + (offset * buffer._stride))
            { }

            buffer_view1d(const rvalue<Pointer<T>> head, const rvalue<Int>& count)
            : Count(count)
            , _stride(1)
            , _data(head)
            {

            }

            buffer_view1d(const rvalue<Pointer<T>> head, const rvalue<Int>& stride, const rvalue<Int>& count)
            : Count(count)
            , _stride(stride)
            , _data(head)
            {

            }

            lvalue<T> operator[](const rvalue<Int>& index)
            {
                return _data[_stride * index];
            }

            rvalue<T> operator[](const rvalue<Int>& index) const
            {
                return _data[_stride * index];
            }

            lvalue<T> First()
            {
                return _data[0];
            }

            rvalue<T> First() const
            {
                return _data[0];
            }

            lvalue<T> Last()
            {
                return _data[_stride * (Count - 1)];
            }

            rvalue<T> Last() const
            {
                return _data[_stride * (Count - 1)];
            }

            const Int Count;
        private:
            const Int _stride;
            Pointer<T> _data;

            // needed to access private _data member
            template<typename>
            friend struct Function;
        };

        template<typename T>
        struct buffer2d
        {
            typedef spark::device_buffer2d<typename T::host_type> host_type;

            // extern constructor
            buffer2d(extern_construct_t)
            : Width(extern_construct)
            , Height(extern_construct)
            , _data(extern_construct)
            {

            }

            buffer2d(buffer2d&& other)
            : Width(std::move(const_cast<Int&>(other.Width)))
            , Height(std::move(const_cast<Int&>(other.Height)))
            ,_data(std::move(other._data))
            {

            }

            rvalue<Pointer<T>> Data()
            {
                return rvalue<Pointer<T>>(_data._node);
            }

            const buffer_view1d<T> operator[](const rvalue<Int>& index) const
            {
                return buffer_view1d<T>(_data + index, Width, Height);
            }

            buffer_view1d<T> operator[](const rvalue<Int>& index)
            {
                return buffer_view1d<T>(_data + index, Width, Height);
            }

            lvalue<T> operator[](const rvalue<Int2>& index)
            {
                return _data[index.Y * Width + index.X];

            }

            rvalue<T> operator[](const rvalue<Int2>& index) const
            {
                return _data[index.Y * Width + index.X];
            }

            const Int Width;
            const Int Height;

        private:
            Pointer<T> _data;
        };
    }
    template<typename T>
    using Buffer2D = client::buffer2d<T>;
    template<typename T>
    using BufferView1D = client::buffer_view1d<T>;
}