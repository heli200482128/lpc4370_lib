#ifndef __CIRCULAR_QUEUE
#define __CIRCULAR_QUEUE

#define QNODE_TYPE bool

typedef enum
{
	CQUEUE_OK = 0,
	CQUEUE_ERROR_ARG,
	CQUEUE_ERROR_INUSE,
	CQUEUE_ERROR_MEM,

	CQUEUE_ERROR_FULL,
	CQUEUE_ERROR_EMPTY,

}CQUEUE_STATE_T;

CQUEUE_STATE_T queue_init(unsigned int queue_length);
CQUEUE_STATE_T queue_uninit();

CQUEUE_STATE_T queue_push(QNODE_TYPE node);
CQUEUE_STATE_T queue_pop(QNODE_TYPE *p_node);


#endif
