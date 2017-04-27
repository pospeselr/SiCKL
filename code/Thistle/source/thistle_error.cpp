#include "thistle.hpp"

#include "thistle_error.hpp"

RUFF_EXPORT void thistle_free_error(thistle_error_t* error)
{
    delete error;
}

RUFF_EXPORT const char* thistle_get_error_message(thistle_error_t* error)
{
    if(error)
    {
        return error->error_message.c_str();
    }
    return nullptr;
}