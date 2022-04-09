#ifndef MEMHANDLING_INCLUDE
#define MEMHANDLING_INCLUDE

#include <stdint.h>
#include <assert.h>

#include <utility>

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;
typedef s32 b32;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef uintptr_t uintptr;

typedef size_t sizet;

typedef float f32;
typedef double f64;

#include "error_handling.h"

/*
TODO: Add concept of scratch memory like 4coder. So basically whenever I need heap memory within a function but don't need the user to worry about it then I just want to use a
global scratch block of some sort
*/

namespace bgz
{
    struct Memory_Partition
    {
        void* baseAddress;
        s64 usedAmount;
        s64 size;
        s32 tempMemoryCount {}; //Number of active temporary memory sub partitions (created w/ BeginTemporaryMemory())
    };
    
    struct _PartitionMap
    {
        Memory_Partition partitions[10];
        s32 keys[10];
        s32 currentCount{};
    };
    
    struct Temporary_Memory
    {
        Memory_Partition* memPartition {};
        s64 initialusedAmountFromMemPartition {};
    };
    
    struct ScopedMemory
    {
        ScopedMemory(bgz::Memory_Partition* memPart);
        ~ScopedMemory();
        
        Memory_Partition* _memPartition{};
        s64 _initialusedAmountFromMemPartition{};
    };
    
    struct MemoryBlock
    {
        bool  initialized { false };
        void* permanentStorage { nullptr };
        void* temporaryStorage { nullptr };
        s32 sizeOfPermanentStorage {};
        s64 sizeOfTemporaryStorage {};
        s64 temporaryStorageUsed {};
        s64 totalSize {};
        _PartitionMap partitionMap{};
    };
    
    void* _AllocSize(bgz::Memory_Partition* memPartition, s64 size);
#define PushType(memPartition, type, count) (type*)_AllocSize(memPartition, ((sizeof(type)) * (count)))
#define PushSize(memPartition, size) _AllocSize(memPartition, size)
    void Free(bgz::Memory_Partition&& memPartition, void* ptrToFree);
    void Release(Memory_Partition&& memPartition);
    
    //TODO: Make it so user doesn't have to specify a permanent store if they don't want
    //TODO: Write tests for this stuff
    void InitMemoryBlock(MemoryBlock&& userDefinedAppMemoryStruct, s64 sizeOfMemory, s32 sizeOfPermanentStore, void* memoryStartAddress);
    Memory_Partition* CreatePartitionFromMemoryBlock(MemoryBlock&& memBlock, s64 size, const char* partName);
    Memory_Partition* GetMemoryPartition(MemoryBlock* memBlock, const char* partName);
    void IsAllTempMemoryCleared(bgz::Memory_Partition memPartition);
    void Release(Memory_Partition&& memPartition);
};

#endif

#ifdef MEMORY_HANDLING_IMPL

namespace bgz
{
    ScopedMemory::ScopedMemory(bgz::Memory_Partition* memPart)
    {
        _memPartition = memPart;
        _initialusedAmountFromMemPartition = memPart->usedAmount;
        
        ++memPart->tempMemoryCount;
    };
    
    ScopedMemory::~ScopedMemory()
    {
        assert(_memPartition->usedAmount >= _initialusedAmountFromMemPartition);
        
        _memPartition->usedAmount = _initialusedAmountFromMemPartition;
        
        assert(_memPartition->tempMemoryCount > 0);
        --_memPartition->tempMemoryCount;
    };
    
    void _InsertPartition(_PartitionMap&& partMap, const char* partName, Memory_Partition memPartToInsert)
    {
        s32 uniqueID {};//TODO: Using this method to come up with keys isn't full proof. Could have a name with same letters in different order and it would produce conflicting keys. Prob need to change.
        for (s32 i {}; partName[i] != 0; ++i)
            uniqueID += partName[i];
        
        partMap.keys[partMap.currentCount] = uniqueID;
        partMap.partitions[partMap.currentCount++] = memPartToInsert;
    };
    
    Memory_Partition* _GetPartition(_PartitionMap* partMap, const char* partName)
    {
        s32 uniqueID {};
        for (s32 i {}; partName[i] != 0; ++i)
            uniqueID += partName[i];
        
        s32 keyIndex { -1 };
        for (s32 i {}; i < partMap->currentCount; ++i)
        {
            if (uniqueID == partMap->keys[i])
            {
                keyIndex = i;
                break;
            }
        };
        
        if (keyIndex == -1)
            assert(1 < 0);//Partition name is either incorrect or requested partition doesn't exist!
        
        return &partMap->partitions[keyIndex];
    };
    
    void InitMemoryBlock(MemoryBlock&& memBlock, s64 sizeOfMemory, s32 sizeOfPermanentStore, void* memoryStartAddress)
    {
        memBlock.sizeOfPermanentStorage = sizeOfPermanentStore;
        memBlock.sizeOfTemporaryStorage = sizeOfMemory - (s64)sizeOfPermanentStore;
        memBlock.totalSize = sizeOfMemory;
        memBlock.permanentStorage = memoryStartAddress;
        
        if(memBlock.sizeOfTemporaryStorage > 0)
            memBlock.temporaryStorage = ((u8*)memBlock.permanentStorage + memBlock.sizeOfPermanentStorage);
    };
    
    //TODO: Alignment
    void* _PointerAddition(void* baseAddress, s64 amountToAdvancePointer)
    {
        void* newAddress {};
        newAddress = ((u8*)baseAddress) + amountToAdvancePointer;
        return newAddress;
    };
    
    Memory_Partition* CreatePartitionFromMemoryBlock(MemoryBlock&& memBlock, s64 size, const char* partName)
    {
        assert(size < memBlock.sizeOfTemporaryStorage);
        assert((size + memBlock.temporaryStorageUsed) < memBlock.sizeOfTemporaryStorage);
        
        Memory_Partition memPartition {};
        memPartition.baseAddress = _PointerAddition(memBlock.temporaryStorage, memBlock.temporaryStorageUsed);
        memPartition.size = size;
        memPartition.usedAmount = 0;
        
        memBlock.temporaryStorageUsed += size;
        _InsertPartition($(memBlock.partitionMap), partName, memPartition);
        
        return _GetPartition(&memBlock.partitionMap, partName);
    };
    
    Memory_Partition* GetMemoryPartition(MemoryBlock* memBlock, const char* partName)
    {
        return _GetPartition(&memBlock->partitionMap, partName);
    };
    
    auto _AllocSize(bgz::Memory_Partition* memPartition, s64 size) -> void*
    {
        assert((memPartition->usedAmount + size) <= memPartition->size);
        
        void* Result = _PointerAddition(memPartition->baseAddress, memPartition->usedAmount);
        memPartition->usedAmount += (size);
        
        return Result;
    };
    
    auto _FreeSize(Memory_Partition&& memPartition, s64 sizeToFree) -> void
    {
        assert(sizeToFree < memPartition.size);
        assert(sizeToFree < memPartition.usedAmount);
        
        memPartition.usedAmount -= sizeToFree;
    };
    
    void IsAllTempMemoryCleared(bgz::Memory_Partition memPartition)
    {
        assert(memPartition.tempMemoryCount == 0);
    }
    
    void Free(bgz::Memory_Partition&& memPart, void* memToFree)
    {
        
    };
    
    void Release(Memory_Partition&& memPartition)
    {
        memPartition.usedAmount = 0;
        memPartition.tempMemoryCount = 0;
    };
}

#endif //MEMORY_HANDLING_IMPL