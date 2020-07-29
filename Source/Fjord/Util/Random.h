#pragma once 

#include "Fjord/Common.h" 

namespace Fjord 
{

    /**
     * Generates random numbers.  
     */ 
    class Random 
    {
    public: 
        Random(uint64 seed); 
        Random(); 

        unsigned long long NextULongLong(); 
        long long NextLongLong(); 

        float NextFloat(); 
        double NextDouble(); 

    private: 
        uint64 Next(); 

        uint64 Seed_; 
    };

}