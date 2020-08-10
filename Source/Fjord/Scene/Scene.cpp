#include "Fjord/Scene/Scene.h" 

#include "Fjord/Graphics/Camera.h" 
#include "Fjord/Graphics/Light.h" 
#include "Fjord/Graphics/MeshContainer.h" 
#include "Fjord/Scene/ComponentPool.h" 
#include "Fjord/Scene/Group.h" 
#include "Fjord/Scene/Transform.h" 

#include <algorithm> 

namespace Fjord 
{

    ComponentId Scene::NextCompId_ = 0; 
    ComponentAllocator* Scene::ComponentAllocators_[]; 
    HashMap<ClassId, ComponentId> Scene::TypeToId_; 

    ComponentPool* ComponentAllocator::CreatePool() 
    {
        return new ComponentPool(ElemSize_, this); 
    }

    Scene::EntityData::EntityData() 
        : ActiveComponents(0) 
        , Salt(0) 
        , Status(EntityData::StatusInvalid)
    {
        for (unsigned i = 0; i < MaxComponentCount; i++) 
        {
            ComponentIndices[i] = 0; 
        }
    }

    Scene::Scene() 
    {
        for (unsigned i = 0; i < MaxComponentCount; i++) 
        {
            Components_[i] = 0; 
        }

        Scene::RegisterComponent<Transform>(); 
        Scene::RegisterComponent<MeshContainer>(); 
        Scene::RegisterComponent<Light>(); 
        Scene::RegisterComponent<Camera>(); 
    }

    Scene::~Scene() 
    {
        for (unsigned i = 0; i < MaxComponentCount; i++) 
        {
            if (Components_[i]) delete Components_[i]; 
        }
    }

    void Scene::UpdateGUI()
    {
        for (auto s : Systems_) 
        {
            s->HandleUpdateGUI();  
        }
    }

    void Scene::Update(float dt) 
    {
        for (auto s : Systems_) 
        {
            s->HandleUpdate(dt); 
        }
    }

    void Scene::Render() 
    {
        for (auto s : Systems_) 
        {
            s->HandleRender(); 
        }
    }

    void Scene::AddSystem(EntitySystem* system) 
    {
        if (system) 
        {
            Systems_.push_back(system); 
            system->HandleAttach(this); 

            std::sort(Systems_.begin(), Systems_.end(), [](Ref<EntitySystem>& a, Ref<EntitySystem>& b) -> bool 
            {
                return a->GetPriority() < b->GetPriority(); 
            });
        }
    }

    void Scene::GetEntities(const EntityFilter& filter, Vector<Entity>& entities) 
    {
        for (unsigned i = 0; i < Entities_.size(); i++) 
        {
            EntityData& data = Entities_[i]; 
            bool match = (data.Status != 0) & 
                ((data.ActiveComponents & filter.GetIncludeMask()) == filter.GetIncludeMask()) & 
                (((data.ActiveComponents & filter.GetAnyMask()) != 0) | (filter.GetAnyMask() == 0)) & 
                ((data.ActiveComponents & filter.GetExcludeMask()) == 0); 

            if (match) 
            {
                Entity e = ((Entity) i) | (((Entity) data.Salt) << 32); 
                entities.push_back(e); 
            }
        }
    }

    Group* Scene::GetGroup(const EntityFilter& filter) 
    {
        return new Group(this, filter); 
    }

    Entity Scene::CreateEntity() 
    {
        Entity e = AvailableEntities_.GetId(); 
        if (Entities_.size() <= e) 
        {
            Entities_.resize(e + 1); 
        }
        EntityData& data = Entities_[e]; 
        data.Status = EntityData::StatusValid; 
        e |= ((uint64) data.Salt) << 32; 
        UpdateVersion_++; 
        AddComponent<Transform>(e); 
        return e; 
    }

    bool Scene::DestroyEntity(Entity e) 
    {
        if (IsEntityValid(e)) 
        {
            uint32 id = GetEntityId(e); 
            EntityData& data = Entities_[id]; 
            for (unsigned i = 0; i < MaxComponentCount; i++) 
            {
                if (data.ActiveComponents & (1ULL << i)) 
                {
                    Components_[i]->RemoveElement(data.ComponentIndices[i]); 
                    data.ComponentIndices[i] = 0; 
                }
            }
            AvailableEntities_.ReturnId(id); 
            data.ActiveComponents = 0; 
            data.Salt++; 
            data.Status = EntityData::StatusInvalid; 
            UpdateVersion_++; 
            return true; 
        }
        else 
        {
            return false; 
        }
    }

    bool Scene::IsEntityValid(Entity e) 
    {
        uint32 id = GetEntityId(e); 
        uint32 salt = GetEntitySalt(e); 
        if (id < Entities_.size()) 
        {
            EntityData& data = Entities_[id]; 
            return (data.Status * (data.Salt == salt)) != 0; 
        }
        else 
        {
            return false; 
        }
    }

    Component* Scene::AddComponent(Entity e, ComponentId cid) 
    {
        FJ_EASSERT(IsEntityValid(e)); 
        FJ_EASSERT(cid < MaxComponentCount); 

        if (!Components_[cid]) Components_[cid] = ComponentAllocators_[cid]->CreatePool(); 

        uint32 id = GetEntityId(e); 
        EntityData& data = Entities_[id]; 

        if (!(data.ActiveComponents & (1ULL << cid))) 
        {
            data.ComponentIndices[cid] = Components_[cid]->AddElement(); 
            data.ActiveComponents |= (1ULL << cid); 
            UpdateVersion_++; 
        }

        return (Component*) Components_[cid]->GetElement(data.ComponentIndices[cid]); 
    }

    Component* Scene::GetComponent(Entity e, ComponentId cid) 
    {
        FJ_EASSERT(IsEntityValid(e)); 
        FJ_EASSERT_MSG(cid < MaxComponentCount, "Invalid component ID: %u", cid); 
        
        if (!Components_[cid]) return nullptr;  

        uint32 id = GetEntityId(e); 
        EntityData& data = Entities_[id]; 

        if (data.ActiveComponents & (1ULL << cid)) 
        {
            return (Component*) Components_[cid]->GetElement(data.ComponentIndices[cid]); 
        }
        else 
        {
            return nullptr; 
        }
    }

    bool Scene::RemoveComponent(Entity e, ComponentId cid) 
    {
        FJ_EASSERT(IsEntityValid(e)); 
        FJ_EASSERT(cid < MaxComponentCount); 

        uint32 id = GetEntityId(e); 
        EntityData& data = Entities_[id]; 

        if (data.ActiveComponents & (1ULL << cid)) 
        {
            Components_[cid]->RemoveElement(data.ComponentIndices[cid]); 
            data.ActiveComponents &= ~(1ULL << cid); 
            data.ComponentIndices[cid] = 0; 
            UpdateVersion_++; 
            return true; 
        }
        else 
        {
            return false; 
        }
    }

    bool Scene::HasAllComponents(Entity e, ComponentMask mask) 
    {
        FJ_EASSERT(IsEntityValid(e)); 
        return (Entities_[GetEntityId(e)].ActiveComponents & mask) == mask; 
    }

    bool Scene::HasAnyComponents(Entity e, ComponentMask mask) 
    {
        FJ_EASSERT(IsEntityValid(e)); 
        return ((Entities_[GetEntityId(e)].ActiveComponents & mask) != 0) | (mask == 0); 
    }

    bool Scene::HasNoComponents(Entity e, ComponentMask mask) 
    {
        FJ_EASSERT(IsEntityValid(e)); 
        return (Entities_[GetEntityId(e)].ActiveComponents & mask) == 0; 
    }

}