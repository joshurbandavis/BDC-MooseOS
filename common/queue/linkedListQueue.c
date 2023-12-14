/**
 * linkedListQueue.c - A linked list queue
 * 
 * Author: Josh Davis and John Kotz
 * Created: October 10, 2018
 * Copyright © 2018 Barely Disguised Cyborgs. All rights reserved.
 */

#include "linkedListQueue.h"
#include <stdlib.h>

typedef struct LinkedListQueueItem {
    struct LinkedListQueueItem *next;
    struct LinkedListQueueItem *prev;
    void *value;
} LinkedListQueueItem_t;

typedef struct LinkedListQueue {
    LinkedListQueueItem_t *front;
    LinkedListQueueItem_t *back;
} LinkedListQueue_t;

LinkedListQueue_t *linkedListQueueCreate() {
    LinkedListQueue_t *queue = malloc(sizeof(LinkedListQueue_t));
    if (queue == NULL) return NULL;

    queue->front = NULL;
    queue->back = NULL;
    
    return queue;
}

int linkedListQueuePush(LinkedListQueue_t *queue, void *value) {
    if (queue == NULL || value == NULL) {
        return -1;
    }

    LinkedListQueueItem_t *item = malloc(sizeof(LinkedListQueueItem_t));
    if (item == NULL) return -1;

    item->value = value;
    if (queue->front == NULL) {
        queue->front = item;
        queue->back = item;
    } else {
        queue->back->next = item;
        item->prev = queue->back;
        queue->back = item;
    }

    return 0;
}

void *linkedListQueuePop(LinkedListQueue_t *queue) {
    if (queue == NULL) return NULL;

    LinkedListQueueItem_t *item = queue->front;
    if (item) {
        queue->front = item->next;
        if (item == queue->back) {
            queue->back = NULL;
        }
        if (item->next != NULL) {
            item->next->prev = NULL;
        }
        void *value = item->value;
        free(item);
        return value;
    } else return NULL;
}

void *linkedListQueuePeek(LinkedListQueue_t *queue) {
    if (queue == NULL) return NULL;

    LinkedListQueueItem_t *item = queue->front;
    if (item) {
        return item->value;
    } else return NULL;
}

bool linkedListQueueIsEmpty(LinkedListQueue_t *queue) {
    if (queue == NULL) return -1;
    return queue->front == NULL && queue->back == NULL;
}

void linkedListQueueIterate(LinkedListQueue_t *queue, void (*func)(void *)) {
    if (queue == NULL || func == NULL) return;
    LinkedListQueueItem_t *next = queue->front;

    while (next != NULL) {
        func(next->value);
        next = next->next;
    }
}

void linkedListQueueDelete(LinkedListQueue_t *queue, void (*deleteFunc)(void *)) {
    if (queue == NULL) return;
    
    while (!linkedListQueueIsEmpty(queue)) {
        void *value = linkedListQueuePop(queue);
        if (deleteFunc != NULL) {
            deleteFunc(value);
        } 
    }

    free(queue);
}
