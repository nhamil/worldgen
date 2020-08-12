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

    Renderer::Renderer() 
    {
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
        Meshes_.clear(); 
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
        graphics->SetClearColor(Color(0, 0, 0)); 
        RTSwap_->Clear(); 
        graphics->SetRenderTarget(RTSwap_->GetDest()); 
        graphics->ResetViewport(); 
        
        for (CameraRenderData& cam : Cameras_) 
        {
            Matrix4 camTfm = cam.Transform; 
            Matrix3 camNorm = Matrix3(Transpose(Inverse(camTfm))); 

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
                for (MeshRenderData& instance : Meshes_) 
                {
                    Mesh* mesh = instance.Mesh; 
                    Material* mat = instance.Material; 
                    Shader* shader = mat->GetShader(); 

                    Matrix4 modelView = cam.Transform * instance.Transform; 
                    Matrix3 normalMat = Matrix3(Transpose(Inverse(modelView))); 

                    mat->ApplyParameters(); 
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
        }

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
            0.1f, 
            1000.0f
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
        Meshes_.push_back(data); 
    }

}