#pragma once 

#include "Fjord/Common.h" 
#include "Fjord/Graphics/Camera.h" 
#include "Fjord/Graphics/Light.h" 
#include "Fjord/Graphics/Material.h" 
#include "Fjord/Graphics/Mesh.h" 
#include "Fjord/Math/Matrix3.h" 
#include "Fjord/Math/Matrix4.h" 

namespace Fjord 
{

    struct MeshRenderData 
    {
        Matrix4 Transform; 
        Ref<Fjord::Mesh> Mesh; 
        Ref<Fjord::Material> Material; 
    };

    struct LightRenderData 
    {
        Vector3 Position; 
        Vector3 Direction; 
        Light LightData; 
    };

    struct CameraRenderData 
    {
        Matrix4 Projection; 
        Matrix4 Transform; 
    };

    class Renderer 
    {
        FJ_NO_COPY(Renderer); 
    public: 
        Renderer(); 
        ~Renderer(); 

        void BeginFrame(); 
        void EndFrame(); 

        void AddCamera(Camera* camera, const Matrix4& transform); 

        void DrawLight(Light* light, const Vector3& position, const Vector3& direction); 

        void DrawMesh(Mesh* mesh, Material* material, const Matrix4& transform); 

    private: 
        Vector<MeshRenderData> Meshes_; 
        Vector<LightRenderData> Lights_; 
        Vector<CameraRenderData> Cameras_; 
    };

}