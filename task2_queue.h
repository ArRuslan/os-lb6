#pragma once

#include <cstdint>
#include <cstdio>
#include <memoryapi.h>

template <typename T>
class Queue {
public:
    Queue() {
        data = static_cast<T*>(VirtualAlloc(nullptr, capacity * sizeof(T), MEM_COMMIT, PAGE_READWRITE));
        if (data == nullptr) {
            printf("Failed to allocate memory for the queue\n");
            exit(1);
        }
        readIndex = 0;
        writeIndex = -1;
    }

    ~Queue() {
        VirtualFree(data, 0, MEM_RELEASE);
    }

    void enqueue(const T& item) {
        if (size == capacity) {
            capacity *= 1.25;
            capacity += 1;
            data = static_cast<T*>(VirtualAlloc(data, capacity * sizeof(T), MEM_COMMIT, PAGE_READWRITE));
            if (data == nullptr) {
                printf("Failed to reallocate memory for the queue!\n");
                exit(1);
            }
        }

        writeIndex = (writeIndex + 1) % capacity;
        data[writeIndex] = item;
        size++;
    }

    T dequeue() {
        if (isEmpty()) {
            printf("Queue is empty!\n");
            exit(1);
        }

        T item = data[readIndex];
        readIndex = (readIndex + 1) % capacity;
        size--;
        return item;
    }

    bool isEmpty() {
        return size == 0;
    }

    size_t getSize() {
        return size;
    }

private:
    T* data;
    size_t capacity = 4;
    size_t size = 0;
    int readIndex;
    int writeIndex;
};
