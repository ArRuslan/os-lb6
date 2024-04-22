#include <cstdint>
#include <iostream>
#ifdef __MINGW32__
#include <memoryapi.h>
#endif

#include "md5.h"
#include "task2_queue.h"
#include "task3_lru.hpp"

#ifdef __MINGW32__
bool isPasswordStrong(const std::string& password) {
    if(password.length() < 8)
        return false;

    uint8_t passwordCheck = 0;
    for(int i = 0; i < password.length(); i++) {
        if(password[i] >= 'a' && password[i] <= 'z')
            passwordCheck |= 0b1;
        else if(password[i] >= 'A' && password[i] <= 'Z')
            passwordCheck |= 0b10;
        else if(password[i] >= '0' && password[i] <= '9')
            passwordCheck |= 0b100;
        else if((password[i] >= '!' && password[i] <= '/') || (password[i] >= ':' && password[i] <= '@') || (password[i] >= '[' && password[i] <= '`') || (password[i] >= '{' && password[i] <= '~'))
            passwordCheck |= 0b1000;
    }

    return passwordCheck == 0b1111;
}

void task1_set_password(uint8_t* hash) {
    std::string password, repeat_password;
    while(true) {
        printf("Enter password: ");
        std::getline(std::cin, password);
        if(!isPasswordStrong(password)) {
            printf("Password is not strong enough!\n");
            continue;
        }
        printf("Repeat password: ");
        std::getline(std::cin, repeat_password);
        if(password != repeat_password)  {
            printf("Passwords do not match!\n");
            continue;
        }
        break;
    }

    md5((uint8_t*)password.c_str(), password.size(), hash);
}

void task1_check_password(const uint8_t* hash) {
    std::string password;
    printf("Enter password to check: ");
    std::getline(std::cin, password);

    auto* hash_to_check = (uint8_t*)VirtualAlloc(nullptr, HASH_SIZE, MEM_COMMIT, PAGE_READWRITE);
    md5((uint8_t*)password.c_str(), password.size(), hash_to_check);
    for(int i = 0; i < 16; i++) {
        if(hash_to_check[i] != hash[i]) {
            VirtualFree(hash_to_check, HASH_SIZE, MEM_DECOMMIT);
            printf("Wrong password!\n");
            return;
        }
    }

    VirtualFree(hash_to_check, HASH_SIZE, MEM_DECOMMIT);

    printf("Password is correct!\n");
}

void task1() {
    auto* hash = (uint8_t*)VirtualAlloc(nullptr, HASH_SIZE, MEM_COMMIT, PAGE_READWRITE);
    VirtualLock(hash, HASH_SIZE);

    task1_set_password(hash);
    task1_check_password(hash);

    VirtualUnlock(hash, HASH_SIZE);
    VirtualFree(hash, HASH_SIZE, MEM_DECOMMIT);
}

void task2() {
    Queue<std::string> queue;
    std::string item;
    while(true) {
        printf("Enter new string item (empty string to stop): ");
        std::getline(std::cin, item);
        if(item.empty())
            break;
        queue.enqueue(item);
    }

    printf("Queue size: %lld\n", queue.getSize());

    while (!queue.isEmpty()) {
        printf("Dequeued: %s\n", queue.dequeue().c_str());
    }
}
#endif

void task3() {
    auto* cache = new LruCache(128, 4, 64);

    uint32_t hits = 0;
    uint32_t misses = 0;

    for(int i = 0; i < 1024 * 32; i++) {
        if(cache->access_cache(randrange(0x00, 0xFFFF)))
            hits++;
        else
            misses++;
    }

    printf("Hits: %d\n", hits);
    printf("Misses: %d\n", misses);

    delete cache;
}

int main() {
    task1();
    task2();
    task3();

    return 0;
}

#ifdef __MINGW32__
template class Queue<std::string>;
#endif