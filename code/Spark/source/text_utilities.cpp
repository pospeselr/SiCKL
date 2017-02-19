#include "spark.hpp"

#include "text_utilities.hpp"

namespace spark
{
    namespace lib
    {
        std::string string_format(const char* format, ...)
        {
            va_list args1, args2;

            va_start (args1, format);
            va_copy(args2, args1);
            auto len = vsnprintf(nullptr, 0, format, args1);
            va_end(args1);

            std::string result(len, 0);
            vsnprintf(const_cast<char*>(result.data()), len+1, format, args2);
            va_end(args2);

            return result;
        }
    }
}