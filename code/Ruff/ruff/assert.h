#ifndef RUFF_PRINT_ASSERT
    #include <stdint.h>
    namespace ruff
    {
        inline void print_assert(const char* source, uint32_t line, const char* msg)
        {
            printf("%s(%u) : %s\n", source, line, msg);
        }
    }
    #define RUFF_PRINT_ASSERT(SRC, LINE, MSG) ::ruff::print_assert(SRC, LINE, MSG)
#endif

#define RUFF_ASSERT(X) do { if(!bool(X)) { RUFF_PRINT_ASSERT(__FILE__, __LINE__, #X); RUFF_DEBUGBREAK(); } } while(0)
#define RUFF_VERIFY(X) RUFF_ASSERT(X)


#define RUFF_STATIC_ASSERT(...) static_assert((__VA_ARGS__), #__VA_ARGS__)