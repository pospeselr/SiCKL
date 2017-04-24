#pragma once

#include <stdexcept>

namespace ruff
{
    // pass into functions of the type:
    // function(..., error_t**)
    // by calling like so:
    // auto val = function(a, b, c, throw_on_error());
    template<
        typename error_t,
        typename error_deleter_t,
        error_deleter_t deleter,
        typename error_tostring_t,
        error_tostring_t to_string>
    struct throw_on_error
    {
    private:
        // throws exception with message from error
        RUFF_NEVER_INLINE
        static void throw_error(error_t* err)
        {
            std::runtime_error ex(to_string(err));
            deleter(err);
            throw ex;
        }

        // performs check to see if error has been set
        RUFF_NEVER_INLINE
        static void handle_error()
        {
            error_t** pError = get_error_address();
            error_t* error = *pError;
            if(error)
            {
                *pError = nullptr;
                throw_error(error);
            }
        }

        RUFF_FORCE_INLINE
        static error_t** get_error_address()
        {
            static error_t* err = nullptr;
            return &err;
        }

    public:

        RUFF_FORCE_INLINE
        ~throw_on_error() noexcept(false)
        {
            handle_error();
        }

        RUFF_FORCE_INLINE
        operator error_t**() const
        {
            return get_error_address();
        }
    };
}