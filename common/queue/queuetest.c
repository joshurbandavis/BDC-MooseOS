
#include "linkedListQueue.h"
#include "queue.h"
#include <stdlib.h>
#include <stdio.h>

void iterateFunc(void *item) {
    int *value = (int *)item;
    printf("got %i... ", *value);
}

void deleteFunc(void *item) {
    int *value = (int *)item;
    printf("delete %i... ", *value);
}

int main(int argc, char const *argv[])
{
    queue_t *queue = queueCreate(5);

    int a,b,c,d,e;
    a = 1;
    b = 2;
    c = 3;
    d = 4;
    e = 5;

    if (!queueIsEmpty(queue)) {
        printf("Queue is not empty before push! FAIL\n");
        return -1;
    } else printf("Queue empty check succeeded...\n");

    queuePush(queue, &a);
    queuePush(queue, &b);

    printf("Queue push succeeded...\n");

    if (queueIsEmpty(queue)) {
        printf("Queue is still empty after push! FAIL\n");
        return -1;
    } else if (queueIsFull(queue)) {
        printf("Queue is at maximum capacity after 2 pushes! FAIL\n");
        return -1;
    } else if (queueLength(queue) != 2) {
        printf("Queue length is not 2! It's %i! FAIL\n", queueLength(queue));
        return -1;
    } else printf("Queue size checks succeeded...\n");
    
    void *item = queuePeek(queue);
    if (item != &a) {
        printf("Queue peek doesn't show 'a' as being first! CONTINUING...\n");
    } else printf("Queue peek succeeded...\n");

    item = queuePop(queue);
    if (item != &a) {
        printf("Queue pop doesn't return 'a'! FAIL\n");
        return -1;
    } else printf("Queue pop succeeded...\n");

    queuePush(queue, &c);
    queuePush(queue, &d);
    queuePush(queue, &e);

    queueIterate(queue, iterateFunc);
    printf("\n");
    queueDelete(queue, deleteFunc);
    printf("\n");

    return 0;
}
