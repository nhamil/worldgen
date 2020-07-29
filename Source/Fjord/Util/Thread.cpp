#include "Fjord/Util/Thread.h" 

#include <chrono> 
#include <thread> 

namespace Fjord 
{

    void Sleep(unsigned long long millis) 
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(millis)); 
    }

}
