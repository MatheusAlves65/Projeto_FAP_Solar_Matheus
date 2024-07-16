#include "circular_buffer.h"

void buffer_init_1(CircularBuffer1 *buffer) {
    buffer->head = 0;
    buffer->tail = 0;
    buffer->count = 0;
}

int buffer_is_full_1(CircularBuffer1 *buffer) {
    return buffer->count == BUFFER_SIZE_1;
}

int buffer_is_empty_1(CircularBuffer1 *buffer) {
    return buffer->count == 0;
}

void buffer_write_1(CircularBuffer1 *buffer, int value) {
    if (!buffer_is_full_1(buffer)) {
        buffer->data[buffer->head] = value;
        buffer->head = (buffer->head + 1) % BUFFER_SIZE_1;
        buffer->count++;
    } else {
        //printf("Buffer 1 is full\n");
    }
}

int buffer_read_1(CircularBuffer1 *buffer) {
    int value = -1;
    if (!buffer_is_empty_1(buffer)) {
        value = buffer->data[buffer->tail];
        buffer->tail = (buffer->tail + 1) % BUFFER_SIZE_1;
        buffer->count--;
    } else {
        printf("Buffer 1 is empty\n");
    }
    return value;
}

void buffer_init_2(CircularBuffer2 *buffer) {
    buffer->head = 0;
    buffer->tail = 0;
    buffer->count = 0;
}

int buffer_is_full_2(CircularBuffer2 *buffer) {
    return buffer->count == BUFFER_SIZE_2;
}

int buffer_is_empty_2(CircularBuffer2 *buffer) {
    return buffer->count == 0;
}

void buffer_write_2(CircularBuffer2 *buffer, float value) {
    if (!buffer_is_full_2(buffer)) {
        buffer->data[buffer->head] = value;
        buffer->head = (buffer->head + 1) % BUFFER_SIZE_2;
        buffer->count++;
    } else {
        printf("Buffer 2 is full\n");
    }
}

float buffer_read_2(CircularBuffer2 *buffer) {
    float value = -1.0;
    if (!buffer_is_empty_2(buffer)) {
        value = buffer->data[buffer->tail];
        buffer->tail = (buffer->tail + 1) % BUFFER_SIZE_2;
        buffer->count--;
    } else {
        printf("Buffer 2 is empty\n");
    }
    return value;
}

float calculate_rms(CircularBuffer1 *buffer) {
    float sum_squares = 0.0;
    for (int i = 0; i < BUFFER_SIZE_1; i++) {
        sum_squares += buffer->data[i] * buffer->data[i];
    }
    return sqrt(sum_squares / BUFFER_SIZE_1);
}
