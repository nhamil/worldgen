#include "Fjord/Scene/Group.h" 

namespace Fjord 
{

    Group::Group(Scene* scene, const EntityFilter& filter) 
        : Filter_(filter) 
        , Scene_(scene) 
        , Entities_() 
        , UpdateVersion_(scene->GetUpdateVersion()) 
    {
        Scene_->GetEntities(Filter_, Entities_); 
    } 

    Vector<Entity> Group::GetEntities() 
    {
        Update(); 
        return Vector<Entity>(Entities_); 
    }

    bool Group::Update() 
    {
        if (UpdateVersion_ != Scene_->GetUpdateVersion()) 
        {
            UpdateVersion_ = Scene_->GetUpdateVersion(); 
            Entities_.clear(); 
            FJ_EASSERT(Scene_.Get()); 
            Scene_->GetEntities(Filter_, Entities_); 
            return true; 
        }
        return false; 
    }

}