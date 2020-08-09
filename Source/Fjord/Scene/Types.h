#pragma once 

#include "Fjord/Common.h" 

namespace Fjord 
{

    /** 
     * ID for a registered component. 
     */ 
    using ComponentId = uint32; 

    /** 
     * Designates which components to look at 
     */ 
    using ComponentMask = uint64; 

    /** 
     * ID for an entity from a scene. IDs are unique for each scene. 
     */ 
    using Entity = uint64; 

    class Component; 
    class ComponentPool; 
    class EntityFilter; 
    /**
     * Logic that iterates over entities in a scene. 
     */ 
    class EntitySystem; 
    class Group; 
    class Scene; 

    inline uint32 GetEntityId(Entity e) 
    {
        return e & 0xFFFFFFFFULL; 
    }

    inline uint32 GetEntitySalt(Entity e) 
    {
        return (e >> 32) & 0xFFFFFFFFULL; 
    }

}