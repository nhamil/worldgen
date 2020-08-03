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

    void SetThreadPriority(ThreadPriority priority) 
    {
        switch (priority) 
        {
            case ThreadPriority::Highest: 
                SDL_SetThreadPriority(SDL_THREAD_PRIORITY_TIME_CRITICAL); 
                break; 
            case ThreadPriority::High: 
                SDL_SetThreadPriority(SDL_THREAD_PRIORITY_HIGH); 
                break; 
            case ThreadPriority::Normal: 
                SDL_SetThreadPriority(SDL_THREAD_PRIORITY_NORMAL); 
                break; 
            case ThreadPriority::Low: 
            case ThreadPriority::Lowest: 
                SDL_SetThreadPriority(SDL_THREAD_PRIORITY_LOW); 
                break; 
        }
    }

    unsigned GetCPUCount() 
    {
        int count = SDL_GetCPUCount(); 
        if (count < 1) count = 1; 
        return count; 
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

    int RunThreadFunc(void* data) 
    {
        Thread* thread = (Thread*) data; 

        if (!thread) 
        {
            FJ_EFERROR("Attempting to run thread that does not exist!"); 
            return -1; 
        }

        SetThreadPriority(thread->GetPriority()); 

        thread->RunInternal(); 
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
        if (Handle_ != nullptr) 
        {
            Interrupt(); 
            Detach(); 
        }
    }

    void Thread::Run() {}

    void Thread::RunInternal() 
    {
        if (Runnable_) 
        {
            Runnable_->Run(); 
        }
        else 
        {
            Run(); 
        }
        Running_ = false; 
    }

    bool Thread::Start() 
    {
        if (Handle_ != nullptr) 
        {
            FJ_EFWARN("Attempted to start thread that is already running"); 
            return false; 
        }

        Running_ = true; 

        Handle_ = SDL_CreateThread(RunThreadFunc, Name_.c_str(), this); 
        return true; 
    }

    bool Thread::IsRunning() 
    {
        return Running_; 
    }

    bool Thread::Join() 
    {
        if (Handle_ == nullptr) 
        {
            FJ_EFWARN("Attempted to join thread that is not running"); 
            return false; 
        }

        SDL_WaitThread((SDL_Thread*) Handle_, nullptr); 
        Handle_ = nullptr; 
        return true; 
    }

    bool Thread::Detach() 
    {
        if (Handle_ == nullptr) 
        {
            FJ_EFWARN("Attempted to detach thread that is not running"); 
            return false; 
        }

        SDL_DetachThread((SDL_Thread*) Handle_); 
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

    void Thread::SetPriority(ThreadPriority p) 
    {
        Priority_ = p; 
    }

    ThreadPriority Thread::GetPriority() 
    {
        return Priority_; 
    }

}
