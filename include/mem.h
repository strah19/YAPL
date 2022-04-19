#ifndef MEM_H
#define MEM_H

#include "common.h"

#include <vector>
#include <set>
#include <map>

class IMemoryManager {
public:
    virtual ~IMemoryManager() = default;

    virtual void* alloc(size_t size) = 0;
    virtual void free(void* chunk) = 0;
};

struct BitEntry {
    
};

class MemoryManager : public IMemoryManager {
public:
    MemoryManager();
    ~MemoryManager();

    void* alloc(size_t size);
    void free(void* chunk);
};

#endif // !MEM_H