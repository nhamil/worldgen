#pragma once 

#include "Fjord/Common.h" 
#include "Fjord/Scene/EntitySystem.h" 

namespace Fjord 
{
    
    class MeshRenderSystem : public EntitySystem 
    {
    public: 
        MeshRenderSystem(); 

        virtual void Render() override; 
    };

    class LightRenderSystem : public EntitySystem 
    {
    public: 
        LightRenderSystem(); 

        virtual void Render() override; 
    };

    class CameraRenderSystem : public EntitySystem 
    {
    public: 
        CameraRenderSystem(); 

        virtual void Render() override; 
    };

}