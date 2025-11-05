#pragma once

#ifdef SS_MUTILS_ON

#include <memory>

struct AllocationTool
{
    uint32_t Allocated = 0;
    uint32_t Freed = 0; 

    uint32_t Memory_Usage() { return Allocated - Freed; }
};

static AllocationTool s_AT;

static void PrintMemoryUsage()
{
    std::cout << "Total memory usage: " << s_AT.Memory_Usage() << " bytes\n";
}

void* operator new(size_t size)
{
    s_AT.Allocated += size;
    PrintMemoryUsage();

    return malloc(size);
}

void* operator delete(void* memory, size_t size)
{
    s_AT.Freed += size;
    PrintMemoryUsage();

    free(memory);
}

#endif