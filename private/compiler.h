#pragma once

#define EXPORT __attribute__((visibility("default")))
#define USED __attribute__((used))

#if !defined(__clang__) && __GNUC__ >= 15
#define NONSTRING __attribute__((nonstring))
#else
#define NONSTRING
#endif
