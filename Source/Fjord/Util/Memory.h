#pragma once 

#include "Fjord/Common.h" 

#include <cstdint> 

namespace Fjord 
{

    /** 
     * Copies memory from a source to a destination. 
     */ 
    void* MemCpy(void* dest, const void* src, unsigned long long count, unsigned long long size); 

    /** 
     * Copies memory from a source to a destination with a byte offset for each element.  
     */ 
    void* MemCpyStride(void* dest, const void* src, unsigned long long count, unsigned long long size, unsigned long long destStride, unsigned long long srcStride); 

}