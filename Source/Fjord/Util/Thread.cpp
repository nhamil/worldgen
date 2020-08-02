#include "Fjord/Util/Thread.h" 

#include <SDL2/SDL.h> 

#include <chrono> 
#include <thread> 

namespace Fjord 
{

    void Sleep(unsigned long long millis) 
    {
        // std::this_thread::sleep_for(std::chrono::milliseconds(millis)); 
        SDL_Delay(millis); 
    }

    Mutex::Mutex() 
    {
        Handle_ = SDL_CreateMutex(); 
    }

    Mutex::~Mutex() 
    {
        SDL_DestroyMutex((SDL_mutex*) Handle_); 
    }

    void Mutex::Lock() 
    {
        SDL_LockMutex((SDL_mutex*) Handle_); 
    }

    void Mutex::Unlock() 
    {
        SDL_UnlockMutex((SDL_mutex*) Handle_); 
    }

    bool Mutex::TryLock() 
    {
        return SDL_TryLockMutex((SDL_mutex*) Handle_) == 0; 
    }

    MutexLock::MutexLock(Mutex& mutex) 
        : Mutex_(mutex) 
    {
        Mutex_.Lock(); 
    }

    MutexLock::~MutexLock() 
    {
        Mutex_.Unlock(); 
    }

    static int RunThreadFunc(void* data) 
    {
        Thread* thread = (Thread*) data; 

        if (!thread) 
        {
            FJ_EFERROR("Attempting to run thread that does not exist!"); 
            return -1; 
        }

        thread->Run(); 
        return 0; 
    } 

    class FuncRunnable : public Runnable 
    {
    public: 
        FuncRunnable(ThreadFunc func) : Func_(func) {} 
        ~FuncRunnable() = default; 

        virtual void Run() override 
        {
            Func_(); 
        }

    private: 
        ThreadFunc Func_;  
    };

    Thread::Thread(const String& name) 
        : Handle_(nullptr) 
        , Runnable_(nullptr) 
    {
        Name_ = (name == "") ? "Unnamed Thread" : name; 
    }
    
    Thread::Thread(const String& name, Runnable* runnable) 
        : Handle_(nullptr) 
        , Runnable_(runnable) 
    {
        Name_ = (name == "") ? "Unnamed Thread" : name; 
    }
    
    Thread::Thread(Runnable* runnable) 
        : Handle_(nullptr) 
        , Runnable_(runnable) 
    {
        Name_ = "Unnamed Thread"; 
    }

    Thread::Thread(ThreadFunc func) 
        : Handle_(nullptr) 
        , Runnable_(new FuncRunnable(func))
    {
        Name_ = "Unnamed Thread"; 
    }

    Thread::Thread(const String& name, ThreadFunc func) 
        : Handle_(nullptr) 
        , Runnable_(new FuncRunnable(func))
    {
        Name_ = (name == "") ? "Unnamed Thread" : name; 
    }

    Thread::~Thread() 
    {
        FJ_EFDEBUG("dtor"); 
        if (Handle_ != nullptr) 
        {
            Interrupt(); 
        }
    }

    void Thread::Run() 
    {
        if (Runnable_) 
        {
            Runnable_->Run(); 
        }
        // Join(); 
    }

    bool Thread::Start() 
    {
        if (Handle_ != nullptr) 
        {
            FJ_EFWARN("Attempted to start thread that is already running"); 
            return false; 
        }

        Handle_ = SDL_CreateThread(RunThreadFunc, Name_.c_str(), this); 
        return true; 
    }

    bool Thread::Join() 
    {
        if (Handle_ == nullptr) 
        {
            FJ_EFWARN("Attempted to join thread that is not running"); 
            return false; 
        }

        Interrupt(); 
        FJ_EFDEBUG("Waiting"); 
        SDL_WaitThread((SDL_Thread*) Handle_, nullptr); 
        FJ_EFDEBUG("Waiting done"); 
        // SDL_DetachThread((SDL_Thread*) Handle_); 
        // TODO look at status
        Handle_ = nullptr; 
        return true; 
    }

    bool Thread::IsInterrupted()  
    {
        MutexLock lock(Mutex_); 
        bool i = Interrupted_; 
        return i; 
    }

    void Thread::Interrupt() 
    {
        MutexLock lock(Mutex_); 
        Interrupted_ = true; 
    }

}
