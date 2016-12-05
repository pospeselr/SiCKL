#pragma once

struct spark_error
{
    spark_error(std::string&& message) : _message(message) {}

    std::string _message;
};

namespace Spark
{
    namespace Internal
    {
        template<typename FUNC>
        void TranslateExceptions(spark_error** outError, FUNC&& func)
        {
            try
            {
                func();
            }
            catch(const std::exception& ex)
            {
                if(outError)
                {
                    *outError = new spark_error(ex.what());
                }
            }
        }
    }
}