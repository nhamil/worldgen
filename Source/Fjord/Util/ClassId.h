#pragma once 

#include "Fjord/Common.h" 

#include <typeinfo> 

namespace Fjord 
{

    using ClassId = uint64; 

    /**
     * Returns a unique ID for any class. 
     */ 
    template <class T> 
    ClassId GetClassId() 
    {
        return reinterpret_cast<ClassId>(&typeid(T)); 
    }

    /** 
     * Returns the unique ID of the class of `instance`. 
     * 
     * Note that this will return the ID of a derived class and not the super class. 
     */ 
    template <class T> 
    ClassId GetClassId(const T& instance) 
    {
        return reinterpret_cast<ClassId>(&typeid(instance)); 
    }

}