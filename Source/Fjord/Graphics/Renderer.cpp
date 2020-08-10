#include "Fjord/Graphics/Renderer.h" 

#include "Fjord/Graphics/Color.h" 
#include "Fjord/Graphics/Graphics.h" 

#include <sstream> 

namespace Fjord 
{

    void PrintMatrix(const String& name, const Matrix4& m) 
    {
        FJ_EFDEBUG("Matrix: %s", name.c_str()); 
        for (int y = 0; y < 4; y++) 
        {
            FJ_EFDEBUG("%0.2f, %0.2f, %0.2f, %0.2f", 
                m[y*4+0], 
                m[y*4+1], 
                m[y*4+2], 
                m[y*4+3] 
            );
        }
    }

    Renderer::Renderer() 
    {

    }

    Renderer::~Renderer() 
    {

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
    }

    void Renderer::EndFrame() 
    {
        FJ_EASSERT(Cameras_.size() <= 1); 

        auto* graphics = GetGraphics(); 
        graphics->SetDepthTest(true); 

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

        graphics->SetBlendMode(BlendMode::SourceAlpha, BlendMode::OneMinusSourceAlpha); 
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