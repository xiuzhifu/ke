#include "spinlock.h"
#include "kiss_queue.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>


#define DEFAULT_QUEUE_SIZE 64
#define MAX_GLOBAL_MQ 0x10000

// 0 means mq is not in global mq.
// 1 means mq is in global mq , or the message is dispatching.

#define MQ_IN_GLOBAL 1
#define MQ_OVERLOAD 1024

struct global_queue {
	struct message_queue *head;
	struct message_queue *tail;
	struct spinlock lock;
};

static struct global_queue *Q = NULL;

void 
kiss_globalmq_push(struct message_queue * queue) {
	struct global_queue *q= Q;

	SPIN_LOCK(q)
	assert(queue->next == NULL);
	if(q->tail) {
		q->tail->next = queue;
		q->tail = queue;
	} else {
		q->head = q->tail = queue;
	}
	SPIN_UNLOCK(q)
}

struct message_queue * 
kiss_globalmq_pop() {
	struct global_queue *q = Q;

	SPIN_LOCK(q)
	struct message_queue *mq = q->head;
	if(mq) {
		q->head = mq->next;
		if(q->head == NULL) {
			assert(mq == q->tail);
			q->tail = NULL;
		}
		mq->next = NULL;
	}
	SPIN_UNLOCK(q)

	return mq;
}

struct message_queue * 
kiss_mq_create(uint32_t handle) {
	struct message_queue *q = kiss_malloc(sizeof(*q));
	q->handle = handle;
	q->cap = DEFAULT_QUEUE_SIZE;
	q->head = 0;
	q->tail = 0;
	SPIN_INIT(q)
	// When the queue is create (always between service create and service init) ,
	// set in_global flag to avoid push it to global queue .
	// If the service init success, skynet_context_new will call skynet_mq_force_push to push it to global queue.
	q->in_global = 0;
	q->release = 0;
	q->overload = 0;
	q->overload_threshold = MQ_OVERLOAD;
	q->queue = kiss_malloc(sizeof(struct kiss_message) * q->cap);
	q->next = NULL;

	return q;
}

static void 
_release(struct message_queue *q) {
	assert(q->next == NULL);
	SPIN_DESTROY(q)
	kiss_free(q->queue);
	kiss_free(q);
}

uint32_t 
kiss_mq_handle(struct message_queue *q) {
	return q->handle;
}

int
kiss_mq_length(struct message_queue *q) {
	int head, tail,cap;

	SPIN_LOCK(q)
	head = q->head;
	tail = q->tail;
	cap = q->cap;
	SPIN_UNLOCK(q)
	
	if (head <= tail) {
		return tail - head;
	}
	return tail + cap - head;
}

int
kiss_mq_overload(struct message_queue *q) {
	if (q->overload) {
		int overload = q->overload;
		q->overload = 0;
		return overload;
	} 
	return 0;
}

int
kiss_mq_pop(struct message_queue *q, struct kiss_message *message) {
	int ret = 1;
	SPIN_LOCK(q)

	if (q->head != q->tail) {
		*message = q->queue[q->head++];
		ret = 0;
		int head = q->head;
		int tail = q->tail;
		int cap = q->cap;

		if (head >= cap) {
			q->head = head = 0;
		}
		int length = tail - head;
		if (length < 0) {
			length += cap;
		}
		while (length > q->overload_threshold) {
			q->overload = length;
			q->overload_threshold *= 2;
		}
	} else {
		// reset overload_threshold when queue is empty
		q->overload_threshold = MQ_OVERLOAD;
	}

	if (ret) {
		q->in_global = 0;
	}
	
	SPIN_UNLOCK(q)

	return ret;
}

static void
expand_queue(struct message_queue *q) {
	struct kiss_message *new_queue = kiss_malloc(sizeof(struct kiss_message) * q->cap * 2);
	int i;
	for (i=0;i<q->cap;i++) {
		new_queue[i] = q->queue[(q->head + i) % q->cap];
	}
	q->head = 0;
	q->tail = q->cap;
	q->cap *= 2;
	
	kiss_free(q->queue);
	q->queue = new_queue;
}

void 
kiss_mq_push(struct message_queue *q, struct kiss_message *message) {
	assert(message);
	SPIN_LOCK(q)

	q->queue[q->tail] = *message;
	if (++ q->tail >= q->cap) {
		q->tail = 0;
	}

	if (q->head == q->tail) {
		expand_queue(q);
	}

	if (q->in_global == 0) {
		q->in_global = MQ_IN_GLOBAL;
		kiss_globalmq_push(q);
	}
	
	SPIN_UNLOCK(q)
}

void 
kiss_mq_init() {
	struct global_queue *q = kiss_malloc(sizeof(*q));
	memset(q,0,sizeof(*q));
	SPIN_INIT(q);
	Q=q;
}

void 
kiss_mq_mark_release(struct message_queue *q) {
	SPIN_LOCK(q)
	assert(q->release == 0);
	q->release = 1;
	if (q->in_global != MQ_IN_GLOBAL) {
		kiss_globalmq_push(q);
	}
	SPIN_UNLOCK(q)
}

static void
_drop_queue(struct message_queue *q) {
	struct kiss_message msg;
	while(!kiss_mq_pop(q, &msg)) {
	
	}
	_release(q);
}

void
kiss_mq_release(struct message_queue *q) {
	SPIN_LOCK(q)
	
	if (q->release) {
		SPIN_UNLOCK(q)
		_drop_queue(q);
	} else {
		kiss_globalmq_push(q);
		SPIN_UNLOCK(q)
	}
}

void *
kiss_malloc(int size){
	return malloc(size);
}

void 
kiss_free(void *p){
	free(p);
}
