#include "Fjord/Scene/EntitySystem.h" 

namespace Fjord 
{

    EntitySystem::EntitySystem(unsigned priority) 
        : Priority_(priority) 
    {

    }

    Vector<Entity> EntitySystem::GetEntities() const 
    { 
        FJ_EASSERT(Group_); 
        return Group_->GetEntities(); 
    }

    void EntitySystem::HandleAttach(Scene* scene) 
    {
        if (Scene_) HandleDetach(); 

        Scene_ = scene; 
        if (Scene_) 
        {
            OnAttach(); 
        }
    }

    void EntitySystem::HandleDetach() 
    {
        OnDetach(); 
        Group_ = nullptr; 
    }

    void EntitySystem::HandleUpdateGUI() 
    {
        UpdateGroup(); 
        UpdateGUI(); 
    }

    void EntitySystem::HandleUpdate(float dt) 
    {
        UpdateGroup(); 
        Update(dt); 
    }

    void EntitySystem::HandleRender() 
    {
        UpdateGroup(); 
        Render(); 
    }

    void EntitySystem::UpdateGroup() 
    {
        if (!Group_ || Filter_ != Group_->GetFilter()) 
        {
            Group_ = Scene_->GetGroup(Filter_); 
        }
    }

}