#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

void initQueue(Queue* q) {
    q->front = NULL;
    q->rear = NULL;
    q->size = 0;
}

int front(Queue* q){
    if(isEmpty(q))
        return -404;

    return q->front->data;
}

int back(Queue* q){
    if(isEmpty(q))
        return -404;
    return q->rear->data;
}

int queueSize(Queue* q){
    return q->size;
}

bool isEmpty(Queue* q) {
    return q->size == 0;
}

void push(Queue* q, int value) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = value;
    newNode->next = NULL;

    if (isEmpty(q)) {
        q->front = newNode;
        q->rear = newNode;
    } else {
        q->rear->next = newNode;
        q->rear = newNode;
    }

    q->size ++;
}

int pop(Queue* q) {
    if (isEmpty(q)) {
        return -404;
    }

    Node* temp = q->front;
    int value = temp->data;
    q->front = q->front->next;

    if (q->front == NULL) {
        q->rear = NULL;
    }

    q->size --;
    free(temp);
    return value;
}

void freeQueue(Queue* q) {
    while (!isEmpty(q)) {
        pop(q);
    }
}

