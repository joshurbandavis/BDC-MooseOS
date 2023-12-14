/**
 * queue.c - A simple queue
 * 
 * Author: Josh Davis and John Kotz
 * Created: October 10, 2018
 * Copyright © 2018 Barely Disguised Cyborgs. All rights reserved.
 */

#include "queue.h"
#include <stdlib.h>
#include <stdbool.h>

typedef struct queue {
    void **items;
    int front;
    int nextEmpty;
    int size;
} queue_t;

queue_t *queueCreate(int size) {
    queue_t *queue = malloc(sizeof(queue_t));
    if (queue == NULL) return NULL;

    queue->items = calloc(size, sizeof(void *));
    if (queue->items == NULL) {
        free(queue);
        return NULL;
    }
    queue->front = 0;
    queue->nextEmpty = 0;
    queue->size = size;

    return queue;
}

int queueLength(queue_t *queue) {
    if (queue->items[queue->front] == NULL) {
        return 0;
    } else if (queue->nextEmpty > queue->front) {
        return queue->nextEmpty - queue->front;
    } else if (queue->nextEmpty == queue->front && queue->items[queue->front] != NULL) {
        return queue->size;
    }
    return queue->nextEmpty - queue->front + queue->size;
}

bool queueIsEmpty(queue_t *queue) {
    if (queue == NULL) return true;
    return queueLength(queue) == 0;
}

bool queueIsFull(queue_t *queue) {
    if (queue == NULL) return false;
    return queueLength(queue) == queue->size;
}

bool queuePush(queue_t *queue, void *item) {
    if (queue == NULL || item == NULL) return false;

    if (queueIsFull(queue)) return false;
    queue->items[queue->nextEmpty] = item;
    queue->nextEmpty += 1;
    return true;
}

void *queuePop(queue_t *queue) {
    if (queue == NULL) return NULL;

    void *item = queue->items[queue->front];
    queue->items[queue->front] = NULL;
    queue->front += 1;
    return item;
}

void *queuePeek(queue_t *queue) {
    if (queue == NULL) return NULL;

    return queue->items[queue->front];
}

void queueIterate(queue_t *queue, void (*func)(void *)) {
    if (queue == NULL || func == NULL) return;

    int spot = queue->front;
    while (spot != queue->nextEmpty) {
        func(queue->items[spot]);
        spot += 1;
    }
}

void queueDelete(queue_t *queue, void (*deleteFunc)(void *)) {
    if (queue == NULL) return;

    while (!queueIsEmpty(queue)) {
        void *item = queuePop(queue);
        if (deleteFunc != NULL) {
            deleteFunc(item);
        }
    }

    free(queue->items);
    free(queue);
}
