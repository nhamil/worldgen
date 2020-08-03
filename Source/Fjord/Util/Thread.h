#pragma once 

#include "Fjord/Common.h" 

namespace Fjord 
{

    /**
     * Sleep the current thread for a number of milliseconds.  
     */ 
    void Sleep(unsigned long long millis); 

    enum class ThreadPriority 
    {
        Lowest, 
        Low, 
        Normal, 
        High, 
        Highest 
    };

    void SetThreadPriority(ThreadPriority priority); 

    unsigned GetCPUCount(); 

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

        bool IsRunning(); 

        bool Start(); 
        bool Join(); 
        bool Detach(); 

        ThreadPriority GetPriority(); 
        void SetPriority(ThreadPriority priority); 

    private: 
        friend int RunThreadFunc(void* data); 

        void RunInternal(); 

        String Name_; 
        void* Handle_; 
        bool Interrupted_ = false; 
        bool Running_ = false; 
        Mutex Mutex_; 
        Ref<Runnable> Runnable_; 
        ThreadPriority Priority_ = ThreadPriority::Normal; 
    };

    template <class T, class S> 
    class LambdaThread : public Thread 
    {
    public: 
        LambdaThread(T&& t, S start, S end) 
            : Lambda_(std::forward<T>(t)) 
            , Start_(start) 
            , End_(end) {} 
        
        virtual void Run() override 
        {
            Lambda_(Start_, End_); 
        }

    private: 
        T Lambda_; 
        S Start_, End_; 
    };

    template <class T, class S> 
    LambdaThread<T, S>* CreateLambdaThread(T&& t, S start, S end) 
    {
        return new LambdaThread<T, S>(std::forward<T>(t), start, end); 
    }

    template <class IndexType, class Function> 
    void ParallelFor(IndexType begin, IndexType end, const Function& func) 
    {
        const unsigned NumThreads = (unsigned) (GetCPUCount() * 1.5); 
        Vector<Thread*> threads; 

        IndexType size = end - begin; 
        IndexType grain = size / NumThreads + (size % NumThreads > 0 ? 1 : 0); 
        if (grain < 1) grain = 1; 

        auto outerLoop = [&func](IndexType start, IndexType end) 
        {
            for (IndexType i = start; i < end; i++) 
            {
                func(i); 
            }
        };

        for (unsigned i = 0; i < NumThreads; i++) 
        {
            IndexType iBegin = begin + i * grain; 
            IndexType iEnd = iBegin + grain; 
            if (iEnd > end) iEnd = end; 
            Thread* t = CreateLambdaThread(outerLoop, iBegin, iEnd); 
            threads.push_back(t); 
            t->SetPriority(ThreadPriority::Normal); 
            t->Start(); 
        }

        for (auto* thread : threads) 
        {
            thread->Join(); 
            delete thread; 
        }
    }

}