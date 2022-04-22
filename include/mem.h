#ifndef MEM_H
#define MEM_H

#include "common.h"
#include "ast.h"

#include <vector>
#include <set>
#include <map>

const int BIT_MAP_SIZE = 1024;
const int INT_SIZE = sizeof(int) * 8;
const int BIT_MAP_ELEMENTS = BIT_MAP_SIZE / INT_SIZE;

class IMemoryManager {
public:
    virtual ~IMemoryManager() = default;

    virtual void* allocate(size_t size) = 0;
    virtual void free(void* chunk) = 0;
};

typedef struct BitMapEntry {
  int Index;
  int BlocksAvailable;
  int BitMap[BIT_MAP_SIZE];

  BitMapEntry():BlocksAvailable(BIT_MAP_SIZE) {
    memset(BitMap,0xff,BIT_MAP_SIZE / sizeof(char)); 
  }

  void SetBit(int position, bool flag);
  void SetMultipleBits(int position, bool flag, int count);
  void SetRangeOfInt(int* element, int msb, int lsb, bool flag);
  Ast* FirstFreeBlock(size_t size);
  Ast* AstObjectAddress(int pos);
  void* Head();
} BitMapEntry;


typedef struct ArrayInfo {
  int MemPoolListIndex;
  int StartPosition;
  int Size;
} ArrayMemoryInfo;

class MemoryManager : public IMemoryManager {
public:
  MemoryManager() {}
  ~MemoryManager() {}
  void* allocate(size_t);
  void free(void*);
  std::vector<void*>& get_memory_pool_list(); 

  std::vector<void*> memory_pool_list;
  std::vector<BitMapEntry> BitMapEntryList;
  std::set<BitMapEntry*> FreeMapEntries;
  std::map<void*, ArrayMemoryInfo> ArrayMemoryList;
private:
  void* AllocateArrayMemory(size_t size);
  void* AllocateChunkAndInitBitMap();
  void SetBlockBit(void* object, bool flag);
  void SetMultipleBlockBits(ArrayMemoryInfo* info, bool flag);
};

#endif // !MEM_H