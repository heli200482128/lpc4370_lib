#include "circular_queue.h"

QNODE_TYPE *g_cqueue = NULL;
static unsigned int cqueue_start = 0;
static unsigned int cqueue_end = 0;
static unsigned int cqueue_length = 0;

CQUEUE_STATE_T queue_init(unsigned int length)
{
	if (length == 0)	return CQUEUE_ERROR_ARG;

	if (g_cqueue != NULL)	return CQUEUE_ERROR_INUSE;

	g_cqueue = (QNODE_TYPE*)malloc(length * sizeof(QNODE_TYPE));
	if (g_cqueue == NULL)	return CQUEUE_ERROR_MEM;
	
	cqueue_length = length;
	cqueue_start = cqueue_end = 0;

	return CQUEUE_OK;
}

CQUEUE_STATE_T queue_uninit()
{
	if (g_cqueue)
	{
		free(g_cqueue);

		g_cqueue = NULL;
		cqueue_length = cqueue_start = cqueue_end = 0;
	}

	return CQUEUE_OK;
}

CQUEUE_STATE_T queue_push(QNODE_TYPE node)
{
	if ((cqueue_end + 1) % cqueue_length == cqueue_start)	return CQUEUE_ERROR_FULL;

	g_cqueue[++]
	return CQUEUE_OK;

}


CQUEUE_STATE_T queue_pop(QNODE_TYPE *p_node);