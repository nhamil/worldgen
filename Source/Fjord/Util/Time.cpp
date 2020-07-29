#include "Fjord/Util/Time.h" 

#include <chrono> 

namespace Fjord 
{

    uint64 GetTimeMillis() 
    {
        return std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1L); 
    }

    uint64 GetTimeNano() 
    {
        return std::chrono::system_clock::now().time_since_epoch() / std::chrono::nanoseconds(1L);  
    }

    static const uint64 StartTime = GetTimeNano(); 

    double GetTimeSeconds() 
    {
        return (GetTimeNano() - StartTime) / 1000000000.0; 
    }

}