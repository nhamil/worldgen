#include "Fjord/Graphics/Renderer.h" 

#include "Fjord/Graphics/Color.h" 
#include "Fjord/Graphics/Graphics.h" 
#include "Fjord/Graphics/Texture2D.h" 
#include "Fjord/Graphics/VertexBuffer.h" 

// TODO remove 
#include "Fjord/Graphics/OpenGL.h" 

#include <sstream> 

namespace Fjord 
{

    Ref<Texture2D> Renderer::WhiteTexture_ = nullptr; 

    Renderer::Renderer() 
    {
        if (!WhiteTexture_) 
        {
            WhiteTexture_ = new Texture2D(1, 1, TextureFormat::RGBA8); 
            uint32 data[] = { 0xFFFFFFFF }; 
            WhiteTexture_->SetData(data); 
            WhiteTexture_->Update(); 
        }

        RTSwap_ = new RenderTargetSwap(); 
        PPPipeline_ = new PostProcessPipeline(); 
    }

    Renderer::~Renderer() 
    {

    }

    PostProcessPipeline* Renderer::GetPostProcessPipeline() 
    {
        return PPPipeline_; 
    }

    void Renderer::SetAmbientColor(const Color& color) 
    {
        AmbientColor_ = color; 
    }

    void Renderer::BeginFrame() 
    {
        OpaqueMeshes_.clear(); 
        TransparentMeshes_.clear();  
        TranslucentMeshes_.clear();  
        Lights_.clear(); 
        Cameras_.clear(); 

        auto* graphics = GetGraphics(); 

        graphics->SetClearColor(Color(0.0f, 0.0f, 0.0f, 0.0f)); 
        graphics->Clear(true, true); 
        // graphics->SetBlendMode(BlendMode::One, BlendMode::One); 
        graphics->SetBlendMode(BlendMode::One, BlendMode::Zero); 
        graphics->SetDepthTest(true); 

        RTSwap_->SetSize(graphics->GetScreenWidth(), graphics->GetScreenHeight()); 
    }

    void Renderer::EndFrame() 
    {
        FJ_EASSERT(Cameras_.size() <= 1); 

        auto* graphics = GetGraphics(); 
        graphics->SetDepthTest(true); 

        // GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, RenderTarget_->GetHandle())); 
        // graphics->GetAPI()->BindFramebuffer(RenderTarget_->GetHandle(), RenderTarget_->GetHandle()); 
        // graphics->SetClearColor(Color(0.6, 0.8, 1.0)); 
        graphics->SetClearColor(Color(0, 0, 0)); 
        RTSwap_->Clear(); 
        graphics->SetRenderTarget(RTSwap_->GetDest()); 
        graphics->ResetViewport(); 

        ShaderInfo shaderInfo; 
        shaderInfo.ReservedTextureUnits["fj_MainTex"] = 0; 
        shaderInfo.AvailableTextureUnit = 1; 
        
        // GLCALL(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE)); 
        GLCALL(glEnable(GL_CULL_FACE)); 

        for (CameraRenderData& cam : Cameras_) 
        {
            Matrix4 camTfm = cam.Transform; 
            Matrix3 camNorm = Matrix3(Transpose(Inverse(camTfm))); 

            // GLCALL(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE)); 

            bool first = true; 
            for (LightRenderData& light : Lights_) 
            {
                if (first) 
                {
                    graphics->SetBlendMode(BlendMode::One, BlendMode::Zero); 
                }
                else 
                {
                    graphics->SetBlendMode(BlendMode::One, BlendMode::One); 
                }
                for (MeshRenderData& instance : OpaqueMeshes_) 
                {
                    Mesh* mesh = instance.Mesh; 
                    Material* mat = instance.Material; 
                    Shader* shader = mat->GetShader(); 

                    Matrix4 modelView = cam.Transform * instance.Transform; 
                    Matrix3 normalMat = Matrix3(Transpose(Inverse(modelView))); 

                    shader->SetTextureUnit("fj_MainTex", 0); 
                    graphics->SetTexture(0, WhiteTexture_); 

                    for (auto it : shaderInfo.ReservedTextureUnits) 
                    {
                        shader->SetTextureUnit(it.first, it.second); 
                        Texture* tex = mat->GetTexture(it.first); 
                        if (!tex) tex = WhiteTexture_; 
                        graphics->SetTexture(it.second, tex); 
                    }

                    mat->ApplyParameters(shaderInfo); 

                    shader->SetMatrix4("fj_Projection", cam.Projection); 
                    shader->SetMatrix4("fj_ModelView", modelView); 
                    shader->SetMatrix3("fj_NormalMatrix", normalMat); 

                    shader->SetVector4("fj_LightData.Ambient", first ? AmbientColor_ : Color::Black); 
                    shader->SetVector4("fj_LightData.Color", light.LightData.GetColor()); 
                    shader->SetVector3("fj_LightData.Position", TransformVector(camTfm, light.Position, true)); 
                    shader->SetVector3("fj_LightData.Direction", camNorm * light.Direction); 
                    shader->SetInt("fj_LightData.Type", (int) light.LightData.GetType()); 
                    shader->SetFloat("fj_LightData.Radius", light.LightData.GetRadius()); 

                    graphics->SetShader(shader); 
                    graphics->SetGeometry(mesh->GetGeometry()); 
                    if (mesh->HasIndices()) 
                    {
                        graphics->DrawIndexed(
                            mesh->GetPrimitive(), 
                            0, 
                            mesh->GetIndexCount()
                        );
                    }
                    else 
                    {
                        graphics->Draw(
                            mesh->GetPrimitive(), 
                            0, 
                            mesh->GetVertexCount()
                        );
                    }
                }
                first = false; 
            }

            GLCALL(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL)); 

            // TODO transparent here 

            // GLCALL(glDisable(GL_CULL_FACE)); 

            // translucent 
            for (LightRenderData& light : Lights_) 
            {
                graphics->SetBlendMode(BlendMode::SourceAlpha, BlendMode::OneMinusSourceAlpha); 

                for (MeshRenderData& instance : TranslucentMeshes_) 
                {
                    Mesh* mesh = instance.Mesh; 
                    Material* mat = instance.Material; 
                    Shader* shader = mat->GetShader(); 

                    Matrix4 modelView = cam.Transform * instance.Transform; 
                    Matrix3 normalMat = Matrix3(Transpose(Inverse(modelView))); 

                    shader->SetTextureUnit("fj_MainTex", 0); 
                    graphics->SetTexture(0, WhiteTexture_); 

                    for (auto it : shaderInfo.ReservedTextureUnits) 
                    {
                        shader->SetTextureUnit(it.first, it.second); 
                        Texture* tex = mat->GetTexture(it.first); 
                        if (!tex) tex = WhiteTexture_; 
                        graphics->SetTexture(it.second, tex); 
                    }

                    mat->ApplyParameters(shaderInfo); 

                    shader->SetMatrix4("fj_Projection", cam.Projection); 
                    shader->SetMatrix4("fj_ModelView", modelView); 
                    shader->SetMatrix3("fj_NormalMatrix", normalMat); 

                    shader->SetVector4("fj_LightData.Ambient", first ? AmbientColor_ : Color::Black); 
                    shader->SetVector4("fj_LightData.Color", light.LightData.GetColor()); 
                    shader->SetVector3("fj_LightData.Position", TransformVector(camTfm, light.Position, true)); 
                    shader->SetVector3("fj_LightData.Direction", camNorm * light.Direction); 
                    shader->SetInt("fj_LightData.Type", (int) light.LightData.GetType()); 
                    shader->SetFloat("fj_LightData.Radius", light.LightData.GetRadius()); 

                    graphics->SetShader(shader); 
                    graphics->SetGeometry(mesh->GetGeometry()); 
                    if (mesh->HasIndices()) 
                    {
                        graphics->DrawIndexed(
                            mesh->GetPrimitive(), 
                            0, 
                            mesh->GetIndexCount()
                        );
                    }
                    else 
                    {
                        graphics->Draw(
                            mesh->GetPrimitive(), 
                            0, 
                            mesh->GetVertexCount()
                        );
                    }
                }
            }
        }

        GLCALL(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL)); 
        GLCALL(glDisable(GL_CULL_FACE)); 

        RTSwap_->Swap(); 
        PPPipeline_->Apply(RTSwap_); 
        RTSwap_->ApplyEffect(RTSwap_->GetSource(), nullptr, nullptr); 

        // graphics->SetRenderTarget(nullptr); 
        // graphics->ResetViewport(); 
        // graphics->SetBlendMode(BlendMode::SourceAlpha, BlendMode::OneMinusSourceAlpha); 
        // graphics->SetGeometry(Quad_); 
        // graphics->SetShader(QuadShader_); 
        // graphics->SetTexture(0, RenderTarget_->GetColorBuffer(0)); 
        // graphics->Draw(Primitive::Triangles, 0, 6); 
    }

    void Renderer::AddCamera(Camera* cam, const Matrix4& tfm) 
    {
        FJ_EASSERT(cam); 

        auto* graphics = GetGraphics(); 

        CameraRenderData data; 
        data.Projection = Matrix4::Perspective(
            cam->GetFOV() * FJ_TO_RAD, 
            graphics->GetAspectRatio(), 
            0.00001f, 
            500.0f
        );
        data.Transform = tfm; 
        Cameras_.push_back(data);
    }

    void Renderer::DrawLight(Light* light, const Vector3& position, const Vector3& direction) 
    {
        FJ_EASSERT(light); 

        LightRenderData data; 
        data.Position = position; 
        data.Direction = direction; 
        data.LightData = *light; 
        Lights_.push_back(data); 
    }

    void Renderer::DrawMesh(Mesh* mesh, Material* mat, const Matrix4& tfm) 
    {
        FJ_EASSERT(mesh); 
        FJ_EASSERT(mat); 

        MeshRenderData data; 
        data.Transform = tfm; 
        data.Mesh = mesh; 
        data.Material = mat; 

        switch (mat->GetRenderQueue()) 
        {
            case RenderQueue::Opaque: 
                OpaqueMeshes_.push_back(data); 
                break; 
            case RenderQueue::Transparent: 
                TransparentMeshes_.push_back(data); 
                break; 
            case RenderQueue::Translucent: 
                TranslucentMeshes_.push_back(data); 
                break; 
            default: 
                FJ_EFERROR("Invalid render queue: %d", mat->GetRenderQueue()); 
        }
    }

}