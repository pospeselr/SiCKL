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
            : Count(extern_construct)
            , _data(extern_construct)
            { }

            // move constructor
            buffer1d(buffer1d&& other)
            : Count(std::move(const_cast<Int&>(other.Count)))
            , _data(std::move(other._data))
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
            : Count(count)
            , _stride(1)
            , _data(buffer.Data())
            { }

            buffer_view1d(buffer1d<T>& buffer, const rvalue<Int>& stride, const rvalue<Int>& count)
            : Count(count)
            , _stride(stride)
            , _data(buffer.Data())
            {

            }

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

            const rvalue<Int> Count;
        private:
            const rvalue<Int> _stride;
            Pointer<T> _data;
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
    using Buffer1D = client::buffer1d<T>;
    template<typename T>
    using Buffer2D = client::buffer2d<T>;
    template<typename T>
    using BufferView1D = client::buffer_view1d<T>;

    template<typename T>
    spark::client::rvalue<T> Dot(Buffer1D<T>& left, Buffer1D<T>& right)
    {
        T result;
        For(Int i : Range<Int>(0, left.Count))
        {
            result = result + left[i] * right[i];
        }
        return result;
    }

    template<typename T>
    spark::client::rvalue<T> Dot(BufferView1D<T>& left, BufferView1D<T>& right)
    {
        T result;
        For(Int i : Range<Int>(0, left.Count))
        {
            result = result + left[i] * right[i];
        }
        return result;
    }
}