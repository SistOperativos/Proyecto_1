#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <errno.h>
#include <ucontext.h>
#include <stdbool.h>
#include <signal.h>
#include <structs.h>
#include <time.h>

#define LTHREAD

#define STACK_SIZE 16384

// Thread fuctions
int lthread_create(TCB*, void*, void *(*function) (void *), void *arg); 
void lthread_exit(void*); 
int lthread_join(int ThreadId, void **result);
int lthread_yield(void);
int lthread_detach(TCB thread);

// Mutex functions
int lmutex_init(lmutex* mutex);
int lmutex_destroy(lmutex* mutex);
int lmutex_lock(lmutex* mutex);
int lmutex_unlock(lmutex* mutex);
int lmutexrylock(lmutex* mutex);
