#include "Fjord/Util/Memory.h" 

#include <string.h> 

namespace Fjord 
{

    void* MemCpy(void* dest, const void* src, unsigned long long count, unsigned long long size)
    {
        return memcpy(dest, src, size * count); 
    }

    void* MemCpyStride(void* dest, const void* src, unsigned long long count, unsigned long long size, unsigned long long destStride, unsigned long long srcStride)
    {
        void* res = dest; 

        for (uint64 i = 0; i < count; i++) 
        {
            memcpy(dest, src, size); 
            dest = ((char*) dest) + destStride; 
            src = ((char*) src) + srcStride;  
        }

        return res; 
    }

}
