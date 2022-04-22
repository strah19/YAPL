#include <string>

#include "mem.h"
#include "ast.h"

MemoryManager gMemoryManager;

void BitMapEntry::SetBit(int position, bool flag) {
  BlocksAvailable += flag ? 1 : -1;
  int elementNo = position / INT_SIZE;
  int bitNo = position % INT_SIZE;
  if(flag)
    BitMap[elementNo] = BitMap[elementNo] | (1 << bitNo);
  else
    BitMap[elementNo] = BitMap[elementNo] & ~(1 << bitNo); 
}

void BitMapEntry::SetMultipleBits(int position, bool flag, int count) {
  BlocksAvailable += flag ? count : -count;
  int elementNo = position / INT_SIZE;
  int bitNo = position % INT_SIZE;

  int bitSize = (count <= INT_SIZE - bitNo) ? count : INT_SIZE - bitNo;  
  SetRangeOfInt(&BitMap[elementNo], bitNo + bitSize - 1, bitNo, flag);
  count -= bitSize;
  if (!count) return;
  
  int i = ++elementNo;
  while(count >= 0) {
    if (count <= INT_SIZE) {
      SetRangeOfInt(&BitMap[i], count - 1, 0, flag);
      return;
    }
    else 
      BitMap[i] = flag ? unsigned (-1) : 0;
    count -= 32; 
    i++;
  }
}
  
void BitMapEntry::SetRangeOfInt(int* element, int msb, int lsb, bool flag) {
  if(flag) {
    int mask = (unsigned(-1) << lsb) & (unsigned(-1) >> INT_SIZE-msb-1);
    *element |= mask;
  }
  else {
    int mask = (unsigned(-1) << lsb) & (unsigned(-1) >> INT_SIZE-msb-1);
    *element &= ~mask;
  }
}

Ast* BitMapEntry::FirstFreeBlock(size_t size) {
  for(int i = 0 ; i < BIT_MAP_ELEMENTS; ++i) {
    if(BitMap[i] == 0)
      continue;            
    
    int result = BitMap[i] & -(BitMap[i]); 

    void* address = 0;
    int basePos = (INT_SIZE * i);
    switch (result) {
      case 0x00000001: return AstObjectAddress(basePos + 0);
      case 0x00000002: return AstObjectAddress(basePos + 1);
      case 0x00000004: return AstObjectAddress(basePos + 2);
      case 0x00000008: return AstObjectAddress(basePos + 3);
      case 0x00000010: return AstObjectAddress(basePos + 4);
      case 0x00000020: return AstObjectAddress(basePos + 5);
      case 0x00000040: return AstObjectAddress(basePos + 6);
      case 0x00000080: return AstObjectAddress(basePos + 7);
      case 0x00000100: return AstObjectAddress(basePos + 8);
      case 0x00000200: return AstObjectAddress(basePos + 9);
      case 0x00000400: return AstObjectAddress(basePos + 10);
      case 0x00000800: return AstObjectAddress(basePos + 11);
      case 0x00001000: return AstObjectAddress(basePos + 12);
      case 0x00002000: return AstObjectAddress(basePos + 13);
      case 0x00004000: return AstObjectAddress(basePos + 14);
      case 0x00008000: return AstObjectAddress(basePos + 15);
      case 0x00010000: return AstObjectAddress(basePos + 16);
      case 0x00020000: return AstObjectAddress(basePos + 17);
      case 0x00040000: return AstObjectAddress(basePos + 18);
      case 0x00080000: return AstObjectAddress(basePos + 19);
      case 0x00100000: return AstObjectAddress(basePos + 20);
      case 0x00200000: return AstObjectAddress(basePos + 21);
      case 0x00400000: return AstObjectAddress(basePos + 22);
      case 0x00800000: return AstObjectAddress(basePos + 23);
      case 0x01000000: return AstObjectAddress(basePos + 24);
      case 0x02000000: return AstObjectAddress(basePos + 25);
      case 0x04000000: return AstObjectAddress(basePos + 26);
      case 0x08000000: return AstObjectAddress(basePos + 27);
      case 0x10000000: return AstObjectAddress(basePos + 28);
      case 0x20000000: return AstObjectAddress(basePos + 29);
      case 0x40000000: return AstObjectAddress(basePos + 30);
      case 0x80000000: return AstObjectAddress(basePos + 31);
      default : break;      
      }
    }
  return 0;
}    

Ast* BitMapEntry::AstObjectAddress(int pos) {
  SetBit(pos, false); 
  return &((static_cast<Ast*>(Head()) + (pos / INT_SIZE))
    [INT_SIZE - ((pos % INT_SIZE) + 1)]);
} 

void* BitMapEntry::Head() {
  return gMemoryManager.get_memory_pool_list()[Index];
}

void* MemoryManager::allocate(size_t size) {
  if(size == sizeof(Ast)) {
    std::set<BitMapEntry*>::iterator freeMapI = FreeMapEntries.begin();
    if(freeMapI != FreeMapEntries.end()) {
      BitMapEntry* mapEntry = *freeMapI;
      return mapEntry->FirstFreeBlock(size);
    }
    else {
      AllocateChunkAndInitBitMap();
      FreeMapEntries.insert(&(BitMapEntryList[BitMapEntryList.size() - 1]));
      return BitMapEntryList[BitMapEntryList.size() - 1].FirstFreeBlock(size);  
    }
  }
  else {
    if(ArrayMemoryList.empty()) 
      return AllocateArrayMemory(size);
    else {
      std::map<void*, ArrayMemoryInfo>::iterator infoI =ArrayMemoryList.begin();
      std::map<void*, ArrayMemoryInfo>::iterator infoEndI = ArrayMemoryList.end();
      while(infoI != infoEndI) { 
        ArrayMemoryInfo info = (*infoI).second;
        if(info.StartPosition != 0)  
          continue;           
        else {
          BitMapEntry* entry = &BitMapEntryList[info.MemPoolListIndex];
          if(entry->BlocksAvailable < (size / sizeof(Ast))) 
            return AllocateArrayMemory(size);
          else {
            info.StartPosition = BIT_MAP_SIZE - entry->BlocksAvailable;
            info.Size = size / sizeof(Ast);
            Ast* baseAddress = static_cast<Ast*>(
            memory_pool_list[info.MemPoolListIndex]) + info.StartPosition;

            ArrayMemoryList[baseAddress] = info;
            SetMultipleBlockBits(&info, false);

            return baseAddress;
          } 
        }
      }
    }
  } 
  return 0;
} 

void* MemoryManager::AllocateArrayMemory(size_t size) {
  void* chunkAddress = AllocateChunkAndInitBitMap();
  ArrayMemoryInfo info;
  info.MemPoolListIndex = memory_pool_list.size() - 1;
  info.StartPosition = 0;
  info.Size = size / sizeof(Ast);
  ArrayMemoryList[chunkAddress] = info;
  SetMultipleBlockBits(&info, false);
  return chunkAddress;
}

void* MemoryManager::AllocateChunkAndInitBitMap() {
  BitMapEntry mapEntry;
  Ast* memoryBeginAddress = reinterpret_cast<Ast*> (new char [sizeof(Ast) * BIT_MAP_SIZE]);
  memory_pool_list.push_back(memoryBeginAddress);
  mapEntry.Index = memory_pool_list.size() - 1;
  BitMapEntryList.push_back(mapEntry);
  return memoryBeginAddress;
}

void MemoryManager::free(void* object) {
  if(ArrayMemoryList.find(object) == ArrayMemoryList.end())
    SetBlockBit(object, true);       
  else {
    ArrayMemoryInfo *info = &ArrayMemoryList[object];
    SetMultipleBlockBits(info, true);
  }
}

void MemoryManager::SetBlockBit(void* object, bool flag) {
  int i = BitMapEntryList.size() - 1;
  for (; i >= 0 ; i--) {
    BitMapEntry* bitMap = &BitMapEntryList[i];
    if ((bitMap->Head() <= object )&& (&(static_cast<Ast*>(bitMap->Head()))[BIT_MAP_SIZE-1] >= object)) {
      int position = static_cast<Ast*>(object) - static_cast<Ast*>(bitMap->Head());
      bitMap->SetBit(position, flag);
      flag ? bitMap->BlocksAvailable++ : bitMap->BlocksAvailable--;
    }
  }
}

void MemoryManager::SetMultipleBlockBits(ArrayMemoryInfo* info, bool flag) {
  BitMapEntry* mapEntry = &BitMapEntryList[info->MemPoolListIndex];
  mapEntry->SetMultipleBits(info->StartPosition, flag, info->Size);
}

std::vector<void*>& MemoryManager::get_memory_pool_list() { 
  return memory_pool_list;
}
