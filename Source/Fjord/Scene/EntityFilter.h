#pragma once 

#include "Fjord/Common.h" 
#include "Fjord/Scene/Types.h" 

namespace Fjord 
{

    class EntityFilter 
    {
    public: 
        bool operator==(const EntityFilter& other) const
        {
            return Include_ == other.Include_ 
                && Any_ == other.Any_ 
                && Exclude_ == other.Exclude_; 
        }

        bool operator!=(const EntityFilter& other) const 
        {
            return !(*this == other); 
        }

        template <class First, class... Rest>
        EntityFilter& Include(); 

        template <class First, class... Rest>
        EntityFilter& Any(); 

        template <class First, class... Rest>
        EntityFilter& Exclude(); 

        ComponentMask GetIncludeMask() const
        {
            return Include_; 
        }

        ComponentMask GetAnyMask() const 
        {
            return Any_; 
        }

        ComponentMask GetExcludeMask() const 
        {
            return Exclude_; 
        }

        inline bool Matches(Scene& scene, Entity e); 

    private: 
        ComponentMask Include_ = 0; 
        ComponentMask Any_ = 0; 
        ComponentMask Exclude_ = 0; 
    };

}

#include "Fjord/Scene/Scene.h" 

namespace Fjord 
{

    template <class First, class... Rest>
    inline EntityFilter& EntityFilter::Include() 
    {
        Include_ |= Scene::GetComponentMask<First, Rest...>(); 
        return *this; 
    }

    template <class First, class... Rest>
    EntityFilter& EntityFilter::Any() 
    {
        Any_ |= Scene::GetComponentMask<First, Rest...>(); 
        return *this; 
    }

    template <class First, class... Rest>
    EntityFilter& EntityFilter::Exclude() 
    {
        Exclude_ |= Scene::GetComponentMask<First, Rest...>(); 
        return *this; 
    }

    inline bool EntityFilter::Matches(Scene& scene, Entity e) 
    {
        return scene.HasAllComponents(e, Include_) & scene.HasAnyComponents(e, Any_) & scene.HasNoComponents(e, Exclude_); 
    }

}