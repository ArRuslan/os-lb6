#pragma once

#include <cstdint>

constexpr int HASH_SIZE = 16;

void md5(const uint8_t *initial_msg, size_t initial_len, uint8_t *digest);