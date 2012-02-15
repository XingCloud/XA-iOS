
#ifndef FRT_APP_config_h
#define FRT_APP_config_h

#ifdef WIN32
#define HAVE_PTHREAD 0
#else
#define HAVE_PTHREAD 1
#endif

#define HAVE_POSIX_SEMAPHORES 0

#if defined WIN32
#ifdef NOMINMAX
#undef NOMINMAX
#endif
#define NOMINMAX
#include <windows.h>
#include <process.h>
#elif HAVE_PTHREAD
#include <pthread.h>
#endif

#include <assert.h>

#endif
