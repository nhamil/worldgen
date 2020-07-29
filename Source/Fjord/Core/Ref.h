#pragma once 

#include "Fjord/Common.h" 

#include <memory> 
#include <cstring> 

#include <iostream> 
#include <typeinfo> 

namespace Fjord 
{

    struct RefCount 
    {
        static const int Deleted = -1; 
        int Count = 0; 
        int WeakCount = 1; 

        bool IsDeleted() const
        {
            return Count == Deleted; 
        }
    };

    class RefCounted 
    {
    public: 
        RefCounted() 
        {
            RefCount_ = new RefCount(); 
        } 

        virtual ~RefCounted() 
        {
            // FJ_EFDEBUG("delete %p", this); 
            FJ_EASSERT(RefCount_->WeakCount > 0); 
            RefCount_->WeakCount--; 
            if (RefCount_->WeakCount == 0) 
            {
                // ref count no longer used anywhere 
                delete RefCount_; 
            }
            else 
            {
                // ref count is still used by a weak ref 
                RefCount_->Count = RefCount::Deleted; 
            }
        } 

        void AddRef() 
        {
            // FJ_EFDEBUG("add ref %p", this); 
            RefCount_->Count++; 
            // FJ_EFDEBUG("add ref %p - done", this); 
        }

        void RemoveRef() 
        {
            // FJ_EFDEBUG("remove ref %p", this); 
            FJ_EASSERT(RefCount_->Count > 0); 
            RefCount_->Count--; 
            if (RefCount_->Count == 0) 
            {
                delete this; 
            }
        }

        RefCount* GetRefCountPtr() 
        {
            return RefCount_; 
        }

    private: 
        RefCount* RefCount_ = nullptr; 
    };

    template <class T> 
    class Ref 
    {
    public: 
        Ref()  
            : Value_(nullptr) 
        {
            // FJ_EFDEBUG("ctor empty"); 
        } 

        Ref(std::nullptr_t)  
            : Value_(nullptr) 
        {
            // FJ_EFDEBUG("ctor null"); 
        } 

        Ref(T* value) 
            : Value_(value) 
        {
            // FJ_EFDEBUG("ctor pointer %p", Value_); 
            AddRef(); 
        }

        Ref(const Ref<T>& ref) 
            : Value_(ref.Value_) 
        {
            // FJ_EFDEBUG("ctor ref %p", Value_); 
            AddRef();   
        }

        explicit operator bool() 
        {
            return Get() != nullptr; 
        }

        explicit operator bool() const 
        {
            return Get() != nullptr; 
        }

        operator T*() 
        {
            return Get(); 
        }

        operator T*() const 
        {
            return Get(); 
        }

        Ref<T>& operator=(std::nullptr_t) 
        {
            // FJ_EFDEBUG("= null"); 
            RemoveRef(); 
            return *this; 
        }

        Ref<T>& operator=(T* value) 
        {
            // FJ_EFDEBUG("= pointer %p -> %p", Value_, value); 
            if (value == Value_) return *this; 
            // FJ_EFDEBUG("not equal"); 
            RemoveRef(); 
            Value_ = value; 
            AddRef(); 
            return *this; 
        }

        Ref<T>& operator=(const Ref<T>& ref) 
        {
            // FJ_EFDEBUG("= pointer %p -> %p", Value_, ref.Value_); 
            if (ref.Value_ == Value_) return *this; 
            // FJ_EFDEBUG("not equal"); 
            RemoveRef(); 
            Value_ = ref.Value_; 
            AddRef(); 
            return *this; 
        }

        ~Ref() 
        {
            // FJ_EFDEBUG("dtor %p", Value_); 
            RemoveRef(); 
        }

        unsigned GetRefCount() const 
        {
            return Value_ ? Value_->GetRefCountPtr()->Count : 0; 
        }

        unsigned GetWeakRefCount() const 
        {
            return Value_ ? Value_->GetRefCountPtr()->WeakCount : 0; 
        }

        T* Get() const 
        {
            return Value_; 
        }

        T* operator->() const 
        {
            return Value_; 
        }

        T& operator*() const 
        {
            return *Value_; 
        }

    private: 
        void AddRef() 
        {
            // FJ_EFDEBUG("adding ref to %p", Value_); 
            if (Value_) 
            {
                // FJ_EFDEBUG("value not null, adding"); 
                Value_->AddRef(); 
            }
        }

        void RemoveRef() 
        {
            // FJ_EFDEBUG("removing ref from %p", Value_); 
            if (Value_) 
            {
                // FJ_EFDEBUG("value not null, removing"); 
                Value_->RemoveRef(); 
                Value_ = nullptr; 
            } 
        }

        T* Value_ = nullptr; 
    };

    template <class T> 
    class WeakRef 
    {
    public: 
        WeakRef() 
            : Value_(nullptr) 
            , RefCount_(nullptr) {} 

        WeakRef(std::nullptr_t) 
            : Value_(nullptr)
            , RefCount_(nullptr) {} 

        WeakRef(T* value) 
            : Value_(value) 
            , RefCount_(value ? value->GetRefCountPtr() : nullptr) 
        {
            AddWeakRef(); 
        }

        WeakRef(const WeakRef<T>& ref) 
            : Value_(ref.Value_) 
            , RefCount_(ref.RefCount_) 
        {
            AddWeakRef();    
        }

        WeakRef(const Ref<T>& ref) 
            : Value_(ref.Get()) 
            , RefCount_(nullptr)
        {
            if (Value_) RefCount_ = Value_->GetRefCountPtr(); 
            AddWeakRef(); 
        }

        WeakRef<T>& operator=(std::nullptr_t) 
        {
            RemoveWeakRef(); 
            return *this; 
        }

        WeakRef<T>& operator=(T* value) 
        {
            if (value == Value_) return *this; 
            
            RemoveWeakRef(); 
            Value_ = value; 
            RefCount_ = value ? value->GetRefCountPtr() : nullptr;  
            AddWeakRef(); 
            return *this; 
        }

        WeakRef<T>& operator=(const WeakRef<T>& ref) 
        {
            if (ref.Value_ == Value_) return *this; 
            
            RemoveWeakRef(); 
            Value_ = ref.Value_; 
            RefCount_ = ref.RefCount_;  
            AddWeakRef(); 
            return *this; 
        }

        WeakRef<T>& operator=(const Ref<T>& ref) 
        {
            if (ref.Value_ == Value_) return *this; 
            
            RemoveWeakRef(); 
            Value_ = ref.Value_; 
            RefCount_ = Value_ ? Value_->GetRefCountPtr : nullptr; 
            AddWeakRef(); 
            return *this; 
        }

        ~WeakRef() 
        {
            RemoveWeakRef(); 
        }

        explicit operator bool() 
        {
            return Get() != nullptr; 
        }

        explicit operator bool() const 
        {
            return Get() != nullptr; 
        }

        operator T*() 
        {
            return Get(); 
        }

        operator T*() const 
        {
            return Get(); 
        }

        unsigned GetRefCount() const 
        {
            return RefCount_ ? RefCount_->Count : 0; 
        }

        unsigned GetWeakRefCount() const 
        {
            return RefCount_ ? RefCount_->WeakCount : 0; 
        }

        bool IsValid() const 
        {
            return RefCount_ ? !RefCount_->IsDeleted() : false; 
        }

        bool IsDeleted() const 
        {
            return RefCount_ ? RefCount_->IsDeleted() : true; 
        }

        T* Get() const 
        {
            return IsDeleted() ? nullptr : Value_; 
        }

        T* operator->() const 
        {
            return Get(); 
        }

        T& operator*() const 
        {
            return *Get(); 
        }

    private: 
        void AddWeakRef() 
        {
            if (RefCount_) RefCount_->WeakCount++; 
        }

        void RemoveWeakRef() 
        {
            if (RefCount_) 
            {
                RefCount_->WeakCount--; 
                Value_ = nullptr; 

                if (RefCount_->IsDeleted() && RefCount_->WeakCount == 0) 
                {
                    delete RefCount_; 
                }
            } 
        }

        T* Value_; 
        RefCount* RefCount_; 
    };

}