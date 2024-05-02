#pragma once

#include <cstdint>
#include <cstring>
#include <deque>
#include <vector>
#include <random>

struct CacheBlock {
    uint64_t start_address;
    void* data;
};

class LruCache {
public:
    LruCache(const uint16_t lines, const uint16_t blocks_per_line, const uint16_t block_size) {
        this->lines = lines;
        this->blocks_per_line = blocks_per_line;
        this->block_size = block_size;

        cache.resize(lines);
        for(int i = 0; i < lines; i++)
            cache[i] = std::deque<CacheBlock*>();
    }

    void* access_cache(const uint64_t real_address) {
        const uint16_t offset = real_address & (block_size - 1);
        const uint16_t line = (real_address >> 6) & (lines - 1);

        const uint64_t block_addr = real_address - offset;

        std::deque<CacheBlock*>& blocks = cache[line];
        if(blocks.empty()) {
            auto* block = static_cast<CacheBlock*>(malloc(sizeof(CacheBlock)));
            memset(block, 0, sizeof(CacheBlock));
            block->start_address = block_addr;

            block->data = malloc(block_size);
            memcpy(block->data, (void*)block_addr, block_size);
            blocks.push_back(block);

            misses++;
            return block->data;
        }

        for(int found_block = blocks.size()-1; found_block >= 0; found_block--) {
            if(blocks[found_block] == nullptr)
                continue;
            if(blocks[found_block]->start_address == block_addr) {
                CacheBlock* block = blocks[found_block];
                blocks.erase(blocks.begin()+found_block);
                blocks.push_back(block);

                hits++;
                return block->data;
            }
        }

        if(blocks.size() == blocks_per_line) {
            CacheBlock* block = blocks[0];
            blocks.pop_front();
            memcpy((void*)block->start_address, block->data, block_size);
            free(block->data);
            free(block);
        }

        auto* new_block = static_cast<CacheBlock*>(malloc(sizeof(CacheBlock)));
        memset(new_block, 0, sizeof(CacheBlock));
        new_block->start_address = block_addr;

        new_block->data = malloc(block_size);
        memcpy(new_block->data, (void*)block_addr, block_size);
        blocks.push_back(new_block);

        misses++;
        return new_block->data;
    }

    uint32_t getHitCount() {
        return hits;
    }

    uint32_t getMissCount() {
        return misses;
    }

    ~LruCache() {
        for(auto& line : cache) {
            while(!line.empty()) {
                CacheBlock* block = line[0];
                line.pop_front();
                memcpy((void*)block->start_address, block->data, block_size);
                free(block->data);
                free(block);
            }
        }
    }

private:
    uint16_t lines;
    uint16_t blocks_per_line;
    uint16_t block_size;
    std::vector<std::deque<CacheBlock*>> cache;

    uint32_t hits = 0;
    uint32_t misses = 0;
};

uint64_t randrange(const uint64_t min, const uint64_t max) {
    std::random_device rd;
    std::mt19937_64 eng(rd());
    std::uniform_int_distribution<uint64_t> distr;

    const uint64_t range = max - min + 1;
    return distr(eng) % range + min;
}