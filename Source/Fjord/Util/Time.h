#pragma once 

#include "Fjord/Common.h" 

namespace Fjord 
{

    /**
     * Returns time in milliseconds. 
     */ 
    uint64 GetTimeMillis(); 

    /** 
     * Returns time in nanoseconds. 
     */ 
    uint64 GetTimeNano(); 

    /**
     * Returns time in seconds. 
     */ 
    double GetTimeSeconds(); 

}