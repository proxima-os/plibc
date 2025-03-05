#pragma once

int __cxa_atexit(void (*func)(void *), void *priv, void *dso);
void __cxa_finalize(void *dso);
