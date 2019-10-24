#include <lthread.h>

// Global variables
static threadsList* ready; 
static threadsList* terminated;
static threadsList* new;
static threadsList* blocked;
static TCB* running; 
static mutexList* mutexes; 
static bool initialized;
static int mutexId;
static bool initializedMutex;

// Time and signal control 
static void blockSIGPROF(void);
static void unblockSIGPROF(void);
static void scheduleHandler(int signum, siginfo_t *nfo, void *context);
static bool signalTimer(void);

// Threads  
static bool initQueues(void);
static bool initFirstContext(void);
static bool setStackTCB(TCB*);
static void handleThreadFunction(void);

static bool initQueues(void){
	if ((ready = createThreadList()) == NULL) {
		return false;
	}
	if ((terminated = createThreadList()) == NULL) {
		deleteThreadList(ready);
		return false;
	}
	if ((new = createThreadList()) == NULL) {
		return false;
	}
	if ((blocked = createThreadList()) == NULL) {
		return false;
	}
	if(!initializedMutex){
		if ((mutexes = newMutexList()) == NULL) {
			return false;
		}
		initializedMutex = true;
		mutexId = 1; // starts ids from 1
	}
	return true;
}

static bool initFirstContext(void){
	TCB *block;
	if ((block = createTCB()) == NULL) {
		return false;
	}
	if (getcontext(&block->context) == -1) {
		deleteTCB(block);
		return false;
	}
	block->quantums = 1;
	running = block;
	lmutex temp = {0, false};
	block->blocked = &temp;
	return true;
}

static bool setStackTCB(TCB* thread){
	void *stack;
	if ((stack = malloc(STACK_SIZE)) == NULL) {
		return false;
	}
	thread->context.uc_stack.ss_size = STACK_SIZE;
	thread->context.uc_stack.ss_sp = stack;
	thread->has_dynamic_stack = false; // fixed stack
	return true;
}

static void handleThreadFunction(void){
	blockSIGPROF(); 
	TCB *this = running;
	unblockSIGPROF();
	void *result = this->function(this->funcArg);
	lthread_exit(result);
}

static void blockSIGPROF(void){
	sigset_t sigprof;
	sigemptyset(&sigprof);
	sigaddset(&sigprof, SIGPROF);
	if (sigprocmask(SIG_BLOCK, &sigprof, NULL) == -1) {
		perror("sigprocmask(SIG_BLOCK)");
		abort();
	}
}

static void unblockSIGPROF(void){
	sigset_t sigprof;
	sigemptyset(&sigprof);
	sigaddset(&sigprof, SIGPROF);
	if (sigprocmask(SIG_UNBLOCK, &sigprof, NULL) == -1) {
		perror("sigprocmask(SIG_UNBLOCK)");
		abort();
	}
}

static void scheduleHandler(int signum, siginfo_t *nfo, void *context){
	int old_errno = errno;
	ucontext_t* stored = &running->context;
	if(ready->size !=0){
		if (addThread(ready, running) != 0) {
			perror("addThread()");
			abort();
		}
		if ((running = removeThread(ready)) == NULL) {
			perror("removeThread()");
			abort();
		}
	} else{ 
		errno = old_errno;
		return;
	}
	errno = old_errno;
	if (swapcontext(stored, &(running->context)) == -1) {
		perror("swapcontext()");
		abort();

	}
}

static bool signalTimer(void){
	sigset_t all;
	sigfillset(&all);
	const struct sigaction quantum = {
		.sa_sigaction = scheduleHandler,
		.sa_mask = all,
		.sa_flags = SA_SIGINFO | SA_RESTART
	};
	struct sigaction old;
	if (sigaction(SIGPROF, &quantum, &old) == -1) {
		perror("sigaction()");
		abort();
	}
	const struct itimerval timer = {
		{ 0, 100 }, 
		{ 0, 1 } 
	};
	if (setitimer(ITIMER_PROF, &timer, NULL) == - 1) {
		if (sigaction(SIGPROF, &old, NULL) == -1) {
			perror("sigaction()");
			abort();
		}
		return false;
	}
	return true;
}

int lthread_create(TCB* thread, void* attr, void *(*function) (void *), void *arg){
	blockSIGPROF();
	if (! initialized) {
		if (! initQueues()) {
			abort();
		}
		if (! initFirstContext()) {
			abort();
		}
		if (! signalTimer()){
			abort();
		}
		initialized = true;
	}
	TCB* newThread;
	if ((newThread = createTCB()) == NULL) {
		return -1;
	}
	if (getcontext(&newThread->context) == -1) {
		deleteTCB(newThread);
		return -1;
	}
	if (! setStackTCB(newThread)) {
		deleteTCB(newThread);
		return -1;
	}
	makecontext(&newThread->context, handleThreadFunction, 1, newThread->id);
	newThread->function = function;
	newThread->funcArg = arg;
	newThread->quantums = 1;
	lmutex temp = {0, false};
	newThread->blocked = &temp;
	if (addThread(ready, newThread) != 0) {
		deleteTCB(newThread);
		return -1;
	}
	unblockSIGPROF(); 
	return 0;
}

void lthread_exit(void *result){
	if (running == NULL) {
		exit(EXIT_SUCCESS);
	}
	blockSIGPROF(); 
	running->funcReturn = result;
	if (addThread(terminated, running) != 0) {
		abort();
	}
	if ((running = removeThread(ready)) == NULL) {
		exit(EXIT_SUCCESS);
	}
	setcontext(&running->context); 
}

int lthread_join(int ThreadId, void **result){
	if (ThreadId < 0) {
		errno = EINVAL;
		return -1;
	}
	while(true){
		blockSIGPROF();
		TCB *block = removeThreadById(terminated, ThreadId);
		unblockSIGPROF();
		if (block != NULL) {
			if (result!=NULL){
				*result = block->funcReturn;
			}
			deleteTCB(block);
			return ThreadId;
		}
	}
}

int lthread_yield(void){
	blockSIGPROF();
	ucontext_t *stored = &running->context;
	if (addThread(ready, running) != 0) {
		perror("addThread()");
		abort();
	}
	if ((running = removeThread(ready)) == NULL) {
		perror("removeThread()");
		abort();
	}
	swapcontext(stored, &(running->context));
	unblockSIGPROF();
	return 0;
}

int lmutex_init(lmutex* mutex){
	blockSIGPROF();
	if(!initializedMutex){
		if ((mutexes = newMutexList()) == NULL) {
			return false;
		}
		initializedMutex = true;
		mutexId = 1; 
	}
	if(mutexes->size == 0){
		mutex->id = mutexId;
		mutex->state = false;
		mutexId++;
		addMutex(mutexes, mutex);
	} else{
		if(mutex->id ==0){
			mutex->id = mutexId;
			mutex->state = false;
			mutexId++;
			addMutex(mutexes, mutex);
		} else {
			struct mutexNode* cursor;
			for(cursor = mutexes->head ; cursor != NULL ; cursor = cursor->next){
				if(cursor->mutex->id == mutex->id){
					return -1;
				}
			}
		}
	}
	unblockSIGPROF();
	return 0;
}

int lmutex_destroy(lmutex* mutex){
	blockSIGPROF();
	if(!initializedMutex){
		if ((mutexes = newMutexList()) == NULL) {
			return false;
		}
		initializedMutex = true;
		mutexId = 1;
	}
	if(mutexes->size != 0){
		if(mutex->id !=0){
			return removeMutex(mutexes, mutex->id);
		}
	}
	unblockSIGPROF();
	return 0;
}

int lmutex_lock(lmutex* mutex){
	blockSIGPROF();
	if(mutexes->size == 0){
		return -1;
	} else{
		if(mutex->id == 0){
			return -1;
		} else {
			if(mutex->state == true){
				running->blocked = mutex;
				ucontext_t *stored = &running->context;
				if (addThread(blocked, running) != 0) {
					perror("addThread()");
					abort();
				}
				if ((running = removeThread(ready)) == NULL) {
					perror("removeThread()");
					abort();
				}
				swapcontext(stored, &(running->context));
			} else {
				lmutex temp = {0, false};
				running->blocked = &temp; 
				mutex->state = true;
			}
		}
	}
	unblockSIGPROF();
	return 0;
}

int lmutexrylock(lmutex* mutex){
	blockSIGPROF();
	if(mutexes->size == 0){
		unblockSIGPROF();
		return -1;
	} else{
		if(mutex->id == 0){
			unblockSIGPROF();
			return -1;
		} else {
			if(mutex->state == true){
				unblockSIGPROF();
				return -1;
			} else {
				lmutex temp = {0, false};
				running->blocked = &temp; // resets the threads mutex
				mutex->state = true;
			}
		}
	}
	unblockSIGPROF();
	return 0;
}

int lmutex_unlock(lmutex* mutex){
	blockSIGPROF();
	if(mutexes->size == 0){
		return -1;
	} else{
		if(mutex->id == 0){
			return -1;
		} else {
			if(mutex->state == true) {
				mutex->state = false;
				if(blocked->size !=0 ){
					struct node* cursor;
					for(cursor = blocked->head ; cursor != NULL ; cursor = cursor->next){
						if(cursor->thread->blocked->id == mutex->id){
							lmutex temp = {0, false};
							cursor->thread->blocked = &temp; 
							if(addThread(ready, removeThreadById(blocked, cursor->thread->id)) == -1){
								perror("removeThread()");
								abort();
							}
							mutex->state = true;
							break;
						}
					}
				}
			}
		}
	}
	unblockSIGPROF();
	return 0;
}