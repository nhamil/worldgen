#include "Fjord/Graphics/RenderSystems.h" 

#include "Fjord/Graphics/Camera.h" 
#include "Fjord/Graphics/Light.h" 
#include "Fjord/Graphics/MeshContainer.h" 
#include "Fjord/Graphics/Renderer.h" 
#include "Fjord/Scene/Transform.h" 

namespace Fjord 
{

    MeshRenderSystem::MeshRenderSystem() : EntitySystem(EntitySystem::PostUpdatePriority)
    {
        IncludeComponent<MeshContainer>(); 
        IncludeComponent<Transform>(); 
    }

    void MeshRenderSystem::Render() 
    {
        auto* scene = GetScene(); 
        auto* renderer = GetRenderer(); 

        for (Entity e : GetEntities()) 
        {
            auto& tfm = scene->GetComponent<Transform>(e); 
            auto& mc = scene->GetComponent<MeshContainer>(e); 

            Matrix4 tfmMatrix = tfm.GetMatrix(); 
            Mesh* mesh = mc.GetMesh(); 
            Material* mat = mc.GetMaterial(); 

            if (mesh) // material can be null, default will be used 
            {
                renderer->DrawMesh(
                    mesh, 
                    mat, 
                    tfmMatrix
                );
            }
        }
    }

    LightRenderSystem::LightRenderSystem() : EntitySystem(EntitySystem::PostUpdatePriority)
    {
        IncludeComponent<Light>(); 
        IncludeComponent<Transform>(); 
    }

    void LightRenderSystem::Render() 
    {
        auto* scene = GetScene(); 
        auto* renderer = GetRenderer(); 

        for (Entity e : GetEntities()) 
        {
            auto& tfm = scene->GetComponent<Transform>(e); 
            auto& light = scene->GetComponent<Light>(e); 

            renderer->DrawLight(
                &light, 
                tfm.GetPosition(), 
                tfm.GetRotation() * Vector3::Forward
            ); 
        }
    }

    CameraRenderSystem::CameraRenderSystem() : EntitySystem(EntitySystem::PostUpdatePriority) 
    {
        IncludeComponent<Camera>(); 
        IncludeComponent<Transform>(); 
    }

    void CameraRenderSystem::Render() 
    {
        auto* scene = GetScene(); 
        auto* renderer = GetRenderer(); 

        for (Entity e : GetEntities()) 
        {
            auto& tfm = scene->GetComponent<Transform>(e); 
            auto& cam = scene->GetComponent<Camera>(e); 

            Matrix4 viewMatrix = Inverse(tfm.GetMatrix()); 

            renderer->AddCamera(
                &cam, 
                viewMatrix
            ); 
        }
    }

}