#pragma once 

#include "Fjord/Core/Ref.h" 

#define FJ_OBJECT(className, superName) \
    public: \
        virtual const char* GetClassName() const { return #className; } \
        static const char* GetClassNameStatic() { return #className; } \
    private: \

namespace Fjord 
{

    // class Object : public RefCounted 
    // {
    //     FJ_OBJECT(Object, Object) 
    // public: 
    //     virtual ~Object() {} 
    // };

}