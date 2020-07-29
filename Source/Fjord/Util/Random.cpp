#include "Fjord/Util/Random.h" 

#include "Fjord/Util/Time.h" 

namespace Fjord 
{

    Random::Random(uint64 seed) 
    {
        Seed_ = seed == 0 ? 0x1234567890ABCDEFULL : seed; 
    }

    Random::Random() 
    {
        Seed_ = GetTimeNano(); 
    }

    uint64 Random::Next() 
    {
        Seed_ ^= Seed_ >> 17; 
        Seed_ ^= Seed_ << 27; 
        Seed_ ^= Seed_ >> 22; 
        return Seed_; 
    }

    unsigned long long Random::NextULongLong() 
    {
        return Next(); 
    }

    long long Random::NextLongLong() 
    {
        return (long long) (Next() & 0x7FFFFFFFFFFFFFFFULL); 
    }

    float Random::NextFloat() 
    {
        return (float) NextDouble(); 
    }

    double Random::NextDouble() 
    {
        return (double) (NextLongLong() & 0x07FFFFFFFFFFFFFFULL) / (0x0800000000000000ULL) * 2 - 1;
    }

}