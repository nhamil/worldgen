#pragma once 

#include "Fjord/Common.h" 
#include "Fjord/Scene/Types.h" 
#include "Fjord/Scene/EntityFilter.h" 

namespace Fjord 
{

    class Group : public RefCounted
    {
        FJ_NO_COPY(Group); 
    public: 
        /**
         * Returns a collection of all entities in the group. 
         * Note that this collection does not update when either entities 
         * or components are added or removed. 
         * 
         * You do not have to call `Update()`, it is handled for you. 
         */ 
        Vector<Entity> GetEntities(); 

        bool Update(); 

        Scene* GetScene() 
        {
            return Scene_; 
        }

        const EntityFilter& GetFilter() 
        {
            return Filter_; 
        }

    private: 
        friend class Scene; 

        Group(Scene* scene, const EntityFilter& filter); 

        EntityFilter Filter_; 
        Scene* Scene_; 
        Vector<Entity> Entities_; 
        uint64 UpdateVersion_; 
    };

}