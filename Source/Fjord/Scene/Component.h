#pragma once 

#include "Fjord/Common.h" 

namespace Fjord 
{

    /** 
     * Base class for all components. 
     */ 
    class Component 
    {
    public: 
        Component() = default;  
        Component(const Component& other) = default; 
        Component& operator=(const Component& other) = default; 
        virtual ~Component() = default; 
    };

}