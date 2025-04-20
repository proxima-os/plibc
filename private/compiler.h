#pragma once

#define EXPORT __attribute__((visibility("default")))
/* Priorities 0-100 are reserved for the implementation, so applications can only use 101+
 * When using shared linking, dependency ordering is enough to ensure libc constructors
 * run before application constructors, but to make sure it works properly with static
 * linking as well we need to have a higher priority than any application ctors/dtors. */
#define LIBINIT __attribute__((constructor(100)))
#define LIBFINI __attribute__((destructor(100)))
#define UNUSED __attribute__((unused))
#define USED __attribute__((used))
