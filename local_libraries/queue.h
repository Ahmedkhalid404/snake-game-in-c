#ifndef QUEUE_H
#define QUEUE_H


#include <stdbool.h>

typedef struct Node{
    int data;
    struct Node* next;

}Node;



typedef struct Queue {
    Node *front;
    Node *rear;
    int size;
}Queue;

void initQueue(Queue* q);
bool isEmpty(Queue* q);
void push(Queue* q, int value);
int pop(Queue* q);
int front(Queue* q);
int back(Queue* q);
void freeQueue(Queue* q);
int queueSize(Queue* q);


#endif

