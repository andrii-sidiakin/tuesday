#ifndef _TUE_ASSERT_HPP_INCLUDED_
#define _TUE_ASSERT_HPP_INCLUDED_

#include <cstdio>
#include <string>

#ifdef TUE_ASSERT_NOOP
#define tue_assert(...) (__VA_OPT__(__VA_ARGS__, ) true)
#else
#define tue_assert(cond, ...)                                                  \
    [](bool b, const char *func __VA_OPT__(, std::string &&msg)) {             \
        if (!b) [[unlikely]] {                                                 \
            std::fprintf(                                                      \
                stderr,                                                        \
                "Assert [ %s ]" __VA_OPT__(": %s") "\n"                        \
                                                   " ~ %s\n"                   \
                                                   " ~ %s, line=%i\n",         \
                #cond __VA_OPT__(, msg.empty() ? "" : msg.c_str()), func,      \
                __FILE__, __LINE__);                                           \
            std::abort();                                                      \
        }                                                                      \
        return b;                                                              \
    }(static_cast<bool>(cond),                                                 \
      __PRETTY_FUNCTION__ __VA_OPT__(, std::string(__VA_ARGS__)))
#endif

#endif
