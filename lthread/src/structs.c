#include <structs.h>

static int next_id;

TCB* createTCB(void){
	TCB *new;
	if ((new = calloc(1, sizeof(TCB))) == NULL) {
		return NULL;
	}
	new->id = next_id++;
	return new;
}

void deleteTCB(TCB* block){
	if (block->has_dynamic_stack){
		free(block->context.uc_stack.ss_sp);
	}
	free(block);
}

threadsList* createThreadList(void){
	threadsList* new;
	if ((new = calloc(1, sizeof(threadsList))) == NULL) {
		return NULL;
	}
	return new;
}

void deleteThreadList(threadsList* list){
	struct node *prev = NULL;
	struct node *cursor = list->head;
	while (cursor != NULL) {
		prev = cursor;
		cursor = cursor->next;
		deleteTCB(prev->thread);
		free(prev);
	}
	free(list);
}

int getSize(const threadsList *queue){
	return queue->size;
}

int addThread(threadsList* list, TCB *elem){
	// Create the new node

	struct node *new;

	if ((new = malloc(sizeof(struct node))) == NULL) {
		return errno;

	}

	new->thread = elem;
	new->next = NULL;

	// Enqueue the new node

	if (list->head == NULL) {
		list->head = new;

	} else {
		struct node *parent = list->head;
		while (parent->next != NULL) {
			parent = parent->next;

		}
		parent->next = new;

	}

	list->size += 1;
	return 0;
}

TCB* removeThread(threadsList* list){
	struct node *old_head = list->head;
	list->head = list->head->next;
	list->size -= 1;
	TCB *retval = old_head->thread;
	free(old_head);
	return retval;
}

TCB* removeThreadById(threadsList* list, int id){
	if (list->head == NULL) {
		return NULL;
	}
	struct node *prev = NULL;
	struct node *cur = list->head;
	while (cur != NULL) {
		if (cur->thread->id == id) {
			if (prev == NULL) {
				list->head = cur->next;
			} else {
				prev->next = cur->next;
			}
			TCB *retval = cur->thread;
			free(cur);
			list->size -= 1;
			return retval;
		}
		prev = cur;
		cur = cur->next;
	}
	return NULL;
}

TCB* getThreadById(threadsList* queue, int id){
	if (queue->head == NULL) {
		return NULL;
	}
	struct node *cur = queue->head;
	while (cur != NULL) {
		if (cur->thread->id == id) {
			TCB *retval = cur->thread;
			return retval;
		}
		cur = cur->next;
	}
	return NULL;
}

mutexList* newMutexList(void){
	mutexList* new;
	if ((new = calloc(1, sizeof(mutexList))) == NULL) {
		return NULL;
	}
	return new;
}

lmutex* getMutexById(mutexList* list, int index){
	lmutex* temp;
	if (list->head == NULL) {
		return NULL;
	}
	struct mutexNode* cur = list->head;
	int tmpIndex = 0;
	while (tmpIndex <= index) {
		if (tmpIndex == index){
			temp = cur->mutex;
			break;
		}
		cur  = cur->next;
		tmpIndex++;
	}
	return temp;
}

int addMutex(mutexList* list, lmutex* mutex){
	struct mutexNode *new;
	if ((new = malloc(sizeof(struct mutexNode))) == NULL) {
		return errno;
	}
	new->mutex = mutex;
	new->next = NULL;
	if (list->head == NULL) {
		list->head = new;
	} else {
		struct mutexNode *parent = list->head;
		while (parent->next != NULL) {
			parent = parent->next;
		}
		parent->next = new;
	}
	list->size++;
	return 0;
}

int removeMutex(mutexList* list, int id){
	if (list->head == NULL) {
		return 0;
	}
	struct mutexNode *prev = NULL;
	struct mutexNode *cur = list->head;
	while (cur != NULL) {
		if (cur->mutex->id == id) {
			if (prev == NULL) {
				list->head = cur->next;
			} else {
				prev->next = cur->next;
			}
			int retval = cur->mutex->id;
			free(cur);
			list->size--;
			return retval;
		}
		prev = cur;
		cur = cur->next;
	}
	return -1;
}