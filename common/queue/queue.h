/**
 * queue.h - A simple queue
 * 
 * Author: Josh Davis and John Kotz
 * Created: October 10, 2018
 * Copyright © 2018 Barely Disguised Cyborgs. All rights reserved.
 */

#include <stdbool.h>

typedef struct queue queue_t;

/**
 * Create a queue
 */
queue_t *queueCreate(int size);

/**
 * The length the queue
 */
int queueLength(queue_t *queue);

/**
 * The queue is empty
 */
bool queueIsEmpty(queue_t *queue);

/**
 * The queue is full
 */
bool queueIsFull(queue_t *queue);

/**
 * Pushes the item onto the queue
 */
bool queuePush(queue_t *queue, void *item);

/**
 * Pop an item from the queue
 */
void *queuePop(queue_t *queue);

/**
 * Peek at the first item in this queue
 */
void *queuePeek(queue_t *queue);

/**
 * Iterate through all items in the queue
 */
void queueIterate(queue_t *queue, void (*func)(void *));

/**
 * Delete the queue
 * Will call deleteFunc on all further items in the queue to allow you to delete them too
 * 
 * - parameter queue: The queue to delete
 * - parameter deleteFunc: Called on each remaing element
 */
void queueDelete(queue_t *queue, void (*deleteFunc)(void *));
