#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define BUFFER_SIZE_1 192
#define BUFFER_SIZE_2 15

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
