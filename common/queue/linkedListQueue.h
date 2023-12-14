/**
 * linkedListQueue.h - A FIFO linked list queue
 * 
 * Author: Josh Davis and John Kotz
 * Created: October 10, 2018
 * Copyright © 2018 Barely Disguised Cyborgs. All rights reserved.
 */

#include <stdbool.h>

typedef struct LinkedListQueue LinkedListQueue_t;

/**
 * Create a linked list queue
 */
LinkedListQueue_t *linkedListQueueCreate();

/**
 * Push a new value onto the queue
 */
int linkedListQueuePush(LinkedListQueue_t *queue, void *value);

/**
 * Pop the next value from the queue
 */
void *linkedListQueuePop(LinkedListQueue_t *queue);

/**
 * Peek at the next value on the queue
 */
void *linkedListQueuePeek(LinkedListQueue_t *queue);

/**
 * Check to see if this queue is empty
 */
bool linkedListQueueIsEmpty(LinkedListQueue_t *queue);

/**
 * Iterate through all items in the queue
 */
void linkedListQueueIterate(LinkedListQueue_t *queue, void (*func)(void *));

/**
 * Delete the queue
 * 
 * - parameter deleteFunc: The function called on any remaining items to allow you to delete them if you want
 *      (can be NULL)
 */
void linkedListQueueDelete(LinkedListQueue_t *queue, void (*deleteFunc)(void *));
