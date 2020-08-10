#pragma once 

#include "Fjord/Common.h" 
#include "Fjord/Scene/ComponentPool.h" 
#include "Fjord/Scene/IdList.h" 
#include "Fjord/Scene/Types.h" 
#include "Fjord/Util/ClassId.h" 

namespace Fjord 
{

    /** 
     * Stores all scene information. 
     * This includes all current entities, components, and systems. 
     */ 
    class Scene
    {
        FJ_NO_COPY(Scene); 
    public: 
        /** 
         * Maximum number of components that can be registered. 
         */ 
        static const ComponentId MaxComponentCount = 64; 

        /** 
         * Designated as the null entity 
         */ 
        static const Entity NullEntity = 0; 

        /** 
         * Registers a component with all scenes. Components must be registered before they are added. 
         * Note that there can be up to `Scene::MaxComponentCount` components registered. 
         */ 
        template <class T> 
        static ComponentId RegisterComponent() 
        {
            ComponentId& id = GetComponentIdInternal<T>(); 
            if (id != MaxComponentCount) 
            {
                return id; 
            }
            else if (NextCompId_ < MaxComponentCount)
            {
                id = NextCompId_++; 
                ComponentAllocators_[id] = new DefaultComponentAllocator<T>(); 
                TypeToId_[GetClassId<T>()] = id; 
                return id; 
            }
            else 
            {
                FJ_EASSERT_RET_MSG(false, MaxComponentCount, "Too many components have been registered"); 
            }
        } 

        /** 
         * Returns the component's ID if registered, `Scene::MaxComponentCount` otherwise. 
         */ 
        template <class T> 
        static ComponentId GetComponentId() 
        {
            return GetComponentIdInternal<T>(); 
        } 

        /** 
         * Returns a bitmask for all components specified. 
         */ 
        template <class First, class... Rest> 
        static ComponentMask GetComponentMask() 
        {
            return GetComponentMaskInternal(Sequence<First, Rest...>{}); 
        } 

        Scene(); 
        ~Scene(); 

        void UpdateGUI(); 
        void Update(float dt); 
        void Render(); 

        /**
         * Adds a system to this scene. 
         */ 
        void AddSystem(EntitySystem* system); 

        uint64 GetUpdateVersion() 
        {
            return UpdateVersion_; 
        }

        void GetEntities(const EntityFilter& filter, Vector<Entity>& entities); 

        Group* GetGroup(const EntityFilter& filter); 

        /** 
         * Creates a new entity used by this scene. 
         */ 
        Entity CreateEntity(); 

        /** 
         * Destroys an entity. Returns true if the entity was valid and thus destroyed. 
         */ 
        bool DestroyEntity(Entity e); 

        bool IsEntityValid(Entity e); 

        Component* AddComponent(Entity e, ComponentId cid); 

        Component* GetComponent(Entity e, ComponentId cid); 

        bool RemoveComponent(Entity e, ComponentId cid); 

        /** 
         * Adds a component to an entity and returns a reference to it. 
         * The entity must be valid. 
         */ 
        template <class T> 
        T& AddComponent(Entity e) 
        {
            return *(T*) AddComponent(e, GetComponentId<T>()); 
        } 

        /** 
         * Gets a component from an entity and returns a reference to it. 
         * The entity must be valid. 
         */ 
        template <class T> 
        T& GetComponent(Entity e) 
        {
            return *(T*) GetComponent(e, GetComponentId<T>()); 
        } 

        /** 
         * Returns a pointer to an entity's component if it exists, `nullptr` otherwise. 
         */ 
        template <class T> 
        T* TryGetComponent(Entity e) 
        {
            return (T*) GetComponent(e, GetComponentId<T>()); 
        } 

        /** 
         * Removes a component from an entity, returning true if the entity and the component existed. 
         */ 
        template <class T> 
        bool RemoveComponent(Entity e) 
        {
            return RemoveComponent(e, GetComponentId<T>()); 
        } 

        bool HasAllComponents(Entity e, ComponentMask mask); 

        bool HasAnyComponents(Entity e, ComponentMask mask); 

        bool HasNoComponents(Entity e, ComponentMask mask); 

        template <class First, class... Rest> 
        bool HasAllComponents(Entity e) 
        {
            return HasAllComponents(e, GetComponentMask<First, Rest...>()); 
        } 

        template <class First, class... Rest> 
        bool HasAnyComponents(Entity e) 
        {
            return HasAnyComponents(e, GetComponentMask<First, Rest...>()); 
        } 

        template <class First, class... Rest> 
        bool HasNoComponents(Entity e) 
        {
            return HasNoComponents(e, GetComponentMask<First, Rest...>()); 
        }

    private: 
        static HashMap<ClassId, ComponentId> TypeToId_; 
        static ComponentAllocator* ComponentAllocators_[MaxComponentCount]; 
        static ComponentId NextCompId_; 

        template <class...> struct Sequence {}; 

        template <class First, class... Rest> 
        static ComponentMask GetComponentMaskInternal(Sequence<First, Rest...> helper) 
        {
            ComponentMask mask = (1ULL << GetComponentId<First>()); 
            if (sizeof...(Rest)) 
            {
                mask |= GetComponentMaskInternal(Sequence<Rest...>{}); 
            }
            return mask; 
        }

        static ComponentMask GetComponentMaskInternal(Sequence<> helper) 
        {
            return 0; 
        }

        template <class T> 
        static ComponentId& GetComponentIdInternal() 
        {
            static ComponentId CompId = MaxComponentCount;
            return CompId;  
        } 

        struct EntityData 
        {
            static const uint32 StatusValid = 1; 
            static const uint32 StatusInvalid = 0; 

            unsigned ComponentIndices[MaxComponentCount]; 
            ComponentMask ActiveComponents; 
            uint32 Salt; 
            uint32 Status; 

            EntityData(); 
        };

        Vector<EntityData> Entities_; 
        Vector<Ref<EntitySystem>> Systems_; 
        ComponentPool* Components_[MaxComponentCount]; 
        IdList<uint32> AvailableEntities_; 
        uint64 UpdateVersion_ = 0; 
    };

}

#include "Fjord/Scene/EntitySystem.h" 