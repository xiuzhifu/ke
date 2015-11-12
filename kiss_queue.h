#ifndef KISS_QUEUE_H
#define KISS_QUEUE_H
#include <stdint.h>
#include <stddef.h>
#include "spinlock.h"
struct kiss_message {
	uint32_t source;
	int session;
	void * data;
	size_t size;
};

//struct message_queue;

struct message_queue {
	struct spinlock lock;
	uint32_t handle;
	int cap;
	int head;
	int tail;
	int release;
	int in_global;
	int overload;
	int overload_threshold;
	struct kiss_message *queue;
	struct message_queue *next;
};

void kiss_globalmq_push(struct message_queue * queue);
struct message_queue * kiss_globalmq_pop(void);

struct message_queue * kiss_mq_create(uint32_t handle);
void kiss_mq_release(struct message_queue *q);
uint32_t kiss_mq_handle(struct message_queue *);
// 0 for success
int kiss_mq_pop(struct message_queue *q, struct kiss_message *message);
void kiss_mq_push(struct message_queue *q, struct kiss_message *message);

// return the length of message queue, for debug
int kiss_mq_length(struct message_queue *q);
void* kiss_malloc(int size);
void kiss_free(void *);

void kiss_mq_init();
#endif