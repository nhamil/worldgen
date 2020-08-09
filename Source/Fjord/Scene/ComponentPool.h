#pragma once 

#include "Fjord/Common.h" 
#include "Fjord/Scene/IdList.h" 

#include <new> 
#include <stdlib.h> 

namespace Fjord 
{

    class ComponentPool; 

    class ComponentAllocator
    {
    public: 
        ComponentAllocator(size_t elemSize) : ElemSize_(elemSize) {} 

        virtual void Allocate(void* prev, size_t prevSize, size_t requestedSize, void** newData, size_t* newSize) = 0; 
        virtual void Deallocate(void* data) = 0; 

        virtual void Construct(void* elem) = 0;
        virtual void Destruct(void* elem) = 0;

        ComponentPool* CreatePool(); 

    private: 
        size_t ElemSize_; 
    };

    template <class T> 
    class DefaultComponentAllocator : public ComponentAllocator 
    {
    public: 
        DefaultComponentAllocator() 
            : ComponentAllocator(sizeof(T)) {} 

        virtual void Allocate(void* prev, size_t prevSize, size_t requestedSize, void** newData, size_t* newSize) 
        {
            *newData = realloc(prev, requestedSize * sizeof(T)); 
            *newSize = requestedSize; 
        }

        virtual void Deallocate(void* data) 
        {
            free(data); 
        } 

        virtual void Construct(void* elem) 
        {
            new(elem) T(); 
        }

        virtual void Destruct(void* elem) 
        {
            ((T*) elem)->~T(); 
        }
    };

    class ComponentPool 
    {
    public: 
        ComponentPool(size_t elemSize, ComponentAllocator* alloc) 
            : Allocator_(alloc) 
            , Available_() 
            , Data_(nullptr) 
            , ElemSize_(elemSize) 
            , Size_(0) 
        {
            Allocator_->Allocate(Data_, Size_, 16, (void**) &Data_, &Size_);
        } 

        ~ComponentPool() 
        {
            Allocator_->Deallocate(Data_); 
        }

        void* GetElement(size_t index) 
        {
            return (void*) &Data_[index * ElemSize_]; 
        }

        size_t AddElement() 
        {
            size_t id = Available_.GetId(); 
            if (id >= Size_) 
            {
                Allocator_->Allocate(Data_, Size_, Size_*2, (void**) &Data_, &Size_);
            }
            Allocator_->Construct(Data_ + id * ElemSize_); 
            return id; 
        }

        void RemoveElement(size_t index) 
        {
            Allocator_->Destruct(Data_ + index * ElemSize_); 
            Available_.ReturnId(index); 
        }

    private: 
        ComponentAllocator* Allocator_; 
        IdList<size_t> Available_; 
        char* Data_; 
        size_t ElemSize_; 
        size_t Size_; 
    };

}