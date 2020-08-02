#pragma once 

#include "Fjord/Common.h" 

namespace Fjord 
{

    /**
     * Sleep the current thread for a number of milliseconds.  
     */ 
    void Sleep(unsigned long long millis); 

    /** 
     * Should only be called by the engine 
     */ 
    void DetachAllThreads(); 

    class Runnable : public RefCounted
    {
    public: 
        Runnable() = default; 
        virtual ~Runnable() = default; 

        virtual void Run() = 0; 
    };

    class Mutex 
    {
    public: 
        Mutex(); 
        ~Mutex(); 

        void Lock(); 
        bool TryLock(); 
        void Unlock(); 

    private: 
        void* Handle_; 
    };

    class MutexLock 
    {
    public: 
        MutexLock(Mutex& mutex); 
        ~MutexLock(); 

    private: 
        Mutex& Mutex_; 
    };

    using ThreadFunc = void (*)(); 

    /**
     * `Join()` must be explicitly called to wait for a 
     * thread to exit. Otherwise, the thread will continue 
     * to run until the application exits. 
     */ 
    class Thread : public Runnable
    {
    public: 
        Thread(const String& name = ""); 
        Thread(const String& name, Runnable* runnable); 
        Thread(Runnable* runnable); 
        Thread(ThreadFunc func); 
        Thread(const String& name, ThreadFunc func); 
        ~Thread(); 

        const String& GetName() const { return Name_; } 

        /** 
         * Runs thread logic. Does NOT start the thread. 
         * To start a thread call `Start()`. 
         */ 
        virtual void Run() override; 

        bool IsInterrupted(); 
        void Interrupt(); 

        bool Start(); 
        bool Join(); 

    private: 
        friend void DetachAllThreads(); 

        void Detach(); 

        String Name_; 
        void* Handle_; 
        bool Interrupted_ = false; 
        Mutex Mutex_; 
        Ref<Runnable> Runnable_; 
    };

}