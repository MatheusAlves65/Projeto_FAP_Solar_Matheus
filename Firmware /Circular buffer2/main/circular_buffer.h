#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#define BUFFER_SIZE_1 192
#define BUFFER_SIZE_2 15



typedef struct {
    int *buffer;     // Array to hold the data
    size_t head;     // Index of the head of the buffer
    size_t tail;     // Index of the tail of the buffer
    size_t max;      // Maximum number of items in the buffer
    bool full;       // Flag to keep track of whether the buffer is full
    size_t count;    // Number of items currently in the buffer
} circular_buffer_t;

typedef struct {

    int data[BUFFER_SIZE_1];

    int head;

    int tail;

    int count;

} CircularBuffer1;


typedef struct {
    float data[BUFFER_SIZE_2];
    int head;
    int tail;
    int count;
} CircularBuffer2;


void buffer_init(circular_buffer_t *cb, size_t size1, size_t size2);
void buffer_init_1(CircularBuffer1 *buffer);
int buffer_is_full_1(CircularBuffer1 *buffer);
int buffer_is_empty_1(CircularBuffer1 *buffer);
void buffer_write_1(CircularBuffer1 *buffer, int value);
int buffer_read_1(CircularBuffer1 *buffer);

void buffer_init_2(CircularBuffer2 *buffer);
int buffer_is_full_2(CircularBuffer2 *buffer);
int buffer_is_empty_2(CircularBuffer2 *buffer);
void buffer_write_2(CircularBuffer2 *buffer, float value);
float buffer_read_2(CircularBuffer2 *buffer);

float calculate_rms(CircularBuffer1 *buffer);

#endif // CIRCULAR_BUFFER_H
