#include <stdio.h> 
#include <stdlib.h>
#include <errno.h>
#include <ucontext.h>
#include <stdbool.h>

// Mutex 
typedef struct {
	int id;
	bool state;
} lmutex;

// Mutexes list
struct mutexNode {
	lmutex* mutex;
	struct mutexNode* next;
};

typedef struct {
	struct mutexNode* head;
	size_t size;
} mutexList;

// Mutex Control
mutexList* newMutexList(void);
lmutex* getMutexById(mutexList*, int);
int addMutex(mutexList*, lmutex*);
int removeMutex(mutexList*, int);

// Thread Control Block
typedef struct {
	int id;
	ucontext_t context;
	bool has_dynamic_stack;
	void *(*function) (void *); 
	void *funcArg;
	void *funcReturn;
	int quantums;
	lmutex* blocked;
} TCB;

// TCB control
TCB* createTCB(void);
void deleteTCB(TCB*);

// Thread List Control 
typedef struct {
	struct node *head;
	size_t size;
} threadsList;

struct node {
	struct node *next;
	TCB *thread;
};

threadsList* createThreadList(void);
void deleteThreadList(threadsList*);
int getSize(const threadsList*);
int addThread(threadsList*, TCB*);
TCB* removeThread(threadsList*);
TCB* removeThreadById(threadsList*, int);
TCB* getThreadById(threadsList*, int);