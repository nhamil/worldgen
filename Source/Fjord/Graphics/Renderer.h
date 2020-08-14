#pragma once 

#include "Fjord/Common.h" 
#include "Fjord/Graphics/Camera.h" 
#include "Fjord/Graphics/Color.h" 
#include "Fjord/Graphics/Geometry.h" 
#include "Fjord/Graphics/Light.h" 
#include "Fjord/Graphics/Material.h" 
#include "Fjord/Graphics/Mesh.h" 
#include "Fjord/Graphics/PostProcess.h" 
#include "Fjord/Graphics/RenderTarget.h" 
#include "Fjord/Graphics/RenderTargetSwap.h" 
#include "Fjord/Graphics/Shader.h" 
#include "Fjord/Graphics/Texture2D.h" 
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

        PostProcessPipeline* GetPostProcessPipeline(); 

        void SetAmbientColor(const Color& color); 

        void AddCamera(Camera* camera, const Matrix4& transform); 

        void DrawLight(Light* light, const Vector3& position, const Vector3& direction); 

        void DrawMesh(Mesh* mesh, Material* material, const Matrix4& transform); 

    private: 
        static Ref<Texture2D> WhiteTexture_; 

        Ref<PostProcessPipeline> PPPipeline_; 
        Ref<RenderTargetSwap> RTSwap_; 
        Color AmbientColor_; 
        Vector<MeshRenderData> Meshes_; 
        Vector<LightRenderData> Lights_; 
        Vector<CameraRenderData> Cameras_; 
    };

}