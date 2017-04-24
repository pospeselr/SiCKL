#pragma once

namespace spark
{
    template<typename T> struct Kernel;

    template<typename T>
    struct device_buffer1d
    {
        device_buffer1d()
        : _count(0)
        , _buffer(nullptr)
        { }

        device_buffer1d(size_t count, const T* data)
        : _count(count)
        , _buffer(spark_create_buffer(size(), data, SPARK_THROW_ON_ERROR()),
            [](spark_buffer_t* buffer)
            {
                spark_destroy_buffer(buffer, SPARK_THROW_ON_ERROR());
            })
        { }

        // zero fill
        device_buffer1d(size_t count) : device_buffer1d(count, nullptr) {}
        template<size_t N>
        device_buffer1d(const T (&arr)[N]) : device_buffer1d(N, arr) {}

        void write(size_t offset, size_t count, const T* data)
        {
            spark_write_buffer(_buffer.get(), sizeof(T) * offset, sizeof(T) * count, data, SPARK_THROW_ON_ERROR());
        }

        void write(size_t count, const T* data)
        {
            this->write(0, count, data);
        }

        void write(const T* data)
        {
            this->write(0, _count, data);
        }

        void zero(size_t offset, size_t count)
        {
            write(offset, count, nullptr);
        }

        void zero(size_t count)
        {
            zero(0, count);
        }

        void zero()
        {
            zero(0, this->_size);
        }

        void read(size_t offset, size_t count, T* dest)
        {
            spark_read_buffer(_buffer.get(), sizeof(T) * offset, sizeof(T) * count, dest, SPARK_THROW_ON_ERROR());
        }

        void read(size_t count, T* dest) const
        {
            read(0, count, dest);
        }

        void read(T* dest) const
        {
            read(0, this->_size, dest);
        }

        int32_t count() const { return _count;}
        size_t size() const { return count() * sizeof(T); }

    private:
        template<typename F>
        friend struct Kernel;

        const size_t _count;
        std::shared_ptr<spark_buffer_t> _buffer;
    };

    template<typename T>
    struct device_buffer2d
    {
        device_buffer2d(size_t width, size_t height, const T* data)
        : _width(width)
        , _height(height)
        , _buffer(spark_create_buffer(sizeof(T) * this->count(), data, SPARK_THROW_ON_ERROR()),
            [](spark_buffer_t* buffer)
            {
                spark_destroy_buffer(buffer, SPARK_THROW_ON_ERROR());
            })
        { }

        device_buffer2d(size_t width, size_t height) : device_buffer2d(width, height, nullptr) {}
        template<size_t M, size_t N>
        device_buffer2d(const T (&arr)[M][N]) : device_buffer2d(N, M, arr) {}

        void write(const T* data)
        {
            spark_write_buffer(_buffer.get(), 0, size(), data, SPARK_THROW_ON_ERROR());
        }

        void read(T* dest) const
        {
            spark_read_buffer(_buffer.get(), 0, size(), dest, SPARK_THROW_ON_ERROR());
        }

        int32_t width() const { return _width; }
        int32_t height() const {return _height; }
        size_t count() const { return _width * _height; }
        size_t size() const { return count() * sizeof(T); }

    private:
        template<typename F>
        friend struct Kernel;

        const size_t _width;
        const size_t _height;
        std::shared_ptr<spark_buffer_t> _buffer;
    };
}