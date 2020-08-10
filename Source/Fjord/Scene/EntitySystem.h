#pragma once 

#include "Fjord/Common.h" 
#include "Fjord/Scene/EntityFilter.h" 

namespace Fjord 
{

    class EntitySystem : public RefCounted 
    {
    public: 
        static const unsigned DefaultPriority = 1000; 
        static const unsigned PreUpdatePriority = 10; 
        static const unsigned HighPriority = 100; 
        static const unsigned LowPriority = 10000; 
        static const unsigned PostUpdatePriority = 100000; 

        EntitySystem(unsigned priority = DefaultPriority); 
        virtual ~EntitySystem() = default; 

        virtual void OnAttach() {} 
        virtual void OnDetach() {} 

        virtual void UpdateGUI() {} 

        virtual void Update(float dt) { (void) dt; };

        virtual void Render() {} 

        unsigned GetPriority() const { return Priority_; } 

        Scene* GetScene() const { return Scene_; } 

        Group* GetGroup() const { return Group_; } 

        const EntityFilter& GetFilter() const { return Filter_; } 

        Vector<Entity> GetEntities() const; 

    protected: 
        template <class T> 
        void IncludeComponent() 
        {
            Filter_.Include<T>(); 
        } 

        template <class T> 
        void ExcludeComponent() 
        {
            Filter_.Exclude<T>(); 
        } 

    private: 
        friend class Scene; 

        void HandleAttach(Scene* scene); 
        void HandleDetach(); 

        void HandleUpdateGUI(); 
        void HandleUpdate(float dt); 
        void HandleRender(); 

        void UpdateGroup(); 

        unsigned Priority_; 
        EntityFilter Filter_; 
        Ref<Group> Group_; 
        Scene* Scene_; 
    };

}

#include "Fjord/Scene/Group.h" 