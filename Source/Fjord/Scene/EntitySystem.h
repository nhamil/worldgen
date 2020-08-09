#pragma once 

#include "Fjord/Common.h" 
#include "Fjord/Scene/EntityFilter.h" 

namespace Fjord 
{

    class EntitySystem : public RefCounted 
    {
    public: 
        EntitySystem(); 
        virtual ~EntitySystem() = default; 

        virtual void OnAttach() {} 
        virtual void OnDetach() {} 

        virtual void UpdateGUI() {} 

        virtual void Update(float dt) { (void) dt; };

        virtual void Render() {} 

        Scene* GetScene() const { return Scene_.Get(); } 

        Group* GetGroup() const { return Group_; } 

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

        EntityFilter Filter_; 
        Ref<Group> Group_; 
        WeakRef<Scene> Scene_; 
    };

}

#include "Fjord/Scene/Group.h" 
#include "Fjord/Scene/Scene.h" 