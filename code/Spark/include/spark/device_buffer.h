#pragma once

namespace spark
{
    template<typename T>
    struct device_buffer_1d
    {
        device_buffer_1d(size_t count, const T* data)
        : _size(count)
        , _buffer(spark_create_buffer(sizeof(T) * count, data, THROW_ON_ERROR()),
            [](spark_buffer_t* buffer)
            {
                spark_destroy_buffer(buffer, THROW_ON_ERROR());
            })
        { }

        // zero fill
        device_buffer_1d(size_t count) : device_buffer_1d(count, nullptr) {}
        template<size_t N>
        device_buffer_1d(const T (&arr)[N]) : device_buffer_1d(N, arr) {}

        void write(size_t offset, size_t count, const T* data)
        {
            spark_write_buffer(this->_buffer, sizeof(T) * offset, sizeof(T) * count, data, THROW_ON_ERROR());
        }

        void write(size_t count, const T* data)
        {
            this->write(0, count, data);
        }

        void write(const T* data)
        {
            this->write(0, this->_size, data);
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
            spark_read_buffer(this->_buffer, sizeof(T) * offset, sizeof(T) * count, dest, THROW_ON_ERROR());
        }

        void read(size_t count, T* dest)
        {
            read(0, count, dest);
        }

        void read(T* dest)
        {
            read(0, this->_size, dest);
        }

        size_t size() const
        {
            return _size;
        }

    private:

        const size_t _size;
        std::shared_ptr<spark_buffer_t> _buffer;
    };
}