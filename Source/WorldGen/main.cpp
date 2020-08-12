#include <Fjord/Core/Entry.h> 
#include <Fjord/Core/Input.h> 
#include <Fjord/Core/Object.h> 
#include <Fjord/Core/UI.h> 
#include <Fjord/Core/Window.h> 
#include <Fjord/Math/MathUtil.h> 
#include <Fjord/Math/Quaternion.h> 
#include <Fjord/Graphics/Camera.h> 
#include <Fjord/Graphics/Font.h> 
#include <Fjord/Graphics/Geometry.h> 
#include <Fjord/Graphics/Graphics.h> 
#include <Fjord/Graphics/IndexBuffer.h> 
#include <Fjord/Graphics/Light.h> 
#include <Fjord/Graphics/Material.h> 
#include <Fjord/Graphics/Mesh.h> 
#include <Fjord/Graphics/MeshContainer.h> 
#include <Fjord/Graphics/PostProcess.h> 
#include <Fjord/Graphics/Renderer.h> 
#include <Fjord/Graphics/RenderSystems.h> 
#include <Fjord/Graphics/RenderTarget.h> 
#include <Fjord/Graphics/RenderTexture.h> 
#include <Fjord/Graphics/Shader.h> 
#include <Fjord/Graphics/SpriteBatch.h> 
#include <Fjord/Graphics/Texture2D.h> 
#include <Fjord/Graphics/VertexBuffer.h> 
#include <Fjord/Scene/Component.h> 
#include <Fjord/Scene/EntityFilter.h> 
#include <Fjord/Scene/EntitySystem.h> 
#include <Fjord/Scene/Group.h> 
#include <Fjord/Scene/Scene.h> 
#include <Fjord/Scene/Transform.h> 
#include <Fjord/Util/ClassId.h> 
#include <Fjord/Util/FileUtil.h> 
#include <Fjord/Util/Random.h> 
#include <Fjord/Util/Thread.h> 
#include <Fjord/Util/Time.h> 

#include "WorldGen/World.h" 
#include "WorldGen/WorldGenerator.h" 
#include "WorldGen/WorldGenSystem.h" 

#include <iostream> 

using namespace Fjord; 
using namespace std; 

class FPSCamera : public Component 
{
public: 
    float RotX = 0.0f; // up-down 
    float RotY = 0.0f; // left-right 
}; 

class RotateTag : public Component 
{
public: 
    float Amount = 1.0f;  
}; 

class OrbitTag : public Component 
{
public: 
    Entity Orbits = Scene::NullEntity; 
    float Distance = 10.0f; 
    float Angle = 0.0f; 
    float Speed = 10.0f; 
};

class FPSCameraSystem : public EntitySystem 
{
public: 
    FPSCameraSystem()
    {
        IncludeComponent<FPSCamera>(); 
        IncludeComponent<Transform>(); 
    }

    virtual void Update(float dt) override
    { 
        auto* scene = GetScene(); 
        auto* input = GetInput(); 

        const float MoveSpeed = 10; 
        const float RotSpeed = 2; 
        const float MouseSpeed = 0.0005; 

        Vector3 move; 
        if (input->GetKey(KeyI)) move += Vector3::Forward; 
        if (input->GetKey(KeyK)) move += Vector3::Backward; 
        if (input->GetKey(KeyJ)) move += Vector3::Left; 
        if (input->GetKey(KeyL)) move += Vector3::Right; 
        if (input->GetKey(KeySpace)) move += Vector3::Up; 
        if (input->GetKey(KeySemicolon)) move += Vector3::Down; 
        if (input->GetKey(KeyQuote)) move *= 10; 
        move *= MoveSpeed * dt; 

        float rotRL = 0; 
        float rotUD = 0; 
        if (input->GetKey(KeyW)) rotUD += 1; 
        if (input->GetKey(KeyS)) rotUD -= 1; 
        if (input->GetKey(KeyA)) rotRL += 1; 
        if (input->GetKey(KeyD)) rotRL -= 1; 
        rotRL *= RotSpeed * dt; 
        rotUD *= RotSpeed * dt; 

        rotRL += MouseSpeed * -input->GetMouseMove().X; 
        rotUD += MouseSpeed * -input->GetMouseMove().Y; 

        for (Entity e : GetEntities()) 
        {
            auto& tfm = scene->GetComponent<Transform>(e); 
            auto& cam = scene->GetComponent<FPSCamera>(e); 

            cam.RotX += rotUD; 
            cam.RotY += rotRL; 

            Quaternion rot = 
                Quaternion::AxisAngle(Vector3::Up, cam.RotY) * 
                Quaternion::AxisAngle(Vector3::Right, cam.RotX); 

            tfm.SetRotation(rot); 
            tfm.SetPosition(
                tfm.GetPosition() + rot * move 
            );
        }
    }
};

class RotateSystem : public EntitySystem 
{
public: 
    RotateSystem()
    {
        IncludeComponent<RotateTag>(); 
        IncludeComponent<Transform>(); 
    }

    virtual void Update(float dt) override 
    {
        auto* scene = GetScene(); 

        for (Entity e : GetEntities()) 
        {
            auto& tfm = scene->GetComponent<Transform>(e); 
            auto& rot = scene->GetComponent<RotateTag>(e); 

            double time = (100 + GetTimeSeconds()) * rot.Amount; 

            Quaternion newRot = Quaternion::FromMatrix4(
                Matrix4::RotationX(1.31 * time) * 
                Matrix4::RotationY(1.63 * time) * 
                Matrix4::RotationZ(1.79 * time) 
            );

            tfm.SetRotation(newRot); 
        }
    }
};

class OrbitSystem : public EntitySystem 
{
public: 
    OrbitSystem()
    {
        IncludeComponent<Transform>(); 
        IncludeComponent<OrbitTag>(); 
    }

    virtual void Update(float dt) override 
    {
        auto* scene = GetScene(); 

        for (Entity e : GetEntities()) 
        {
            auto& orbit = scene->GetComponent<OrbitTag>(e); 
            auto& tfm = scene->GetComponent<Transform>(e); 

            orbit.Angle += orbit.Speed * dt; 

            if (scene->IsEntityValid(orbit.Orbits)) 
            {
                if (scene->TryGetComponent<Transform>(orbit.Orbits)) 
                {
                    Quaternion rot = Quaternion::AxisAngle(Vector3::Up, orbit.Angle); 
                    Vector3 pos = rot * Vector3::Forward * orbit.Distance; 
                    tfm.SetPosition(pos); 
                    tfm.SetRotation(rot); 
                } 
                else 
                {
                    FJ_WARN("Attempting to orbit an entity without a transform"); 
                }
            }
            else 
            {
                FJ_WARN("Attempting to orbit an entity that does not exist"); 
            }
        }
    };
};

class Main : public Application 
{
public: 
    virtual void Init() override; 
    virtual void PreUpdate(float dt) override; 
    virtual void PreRender() override; 
    virtual void PostRender() override; 

    Ref<RenderTarget> RT; 
}; 

void Main::Init() 
{
    auto* scene = GetScene(); 

    auto* pipeline = GetRenderer()->GetPostProcessPipeline(); 
    pipeline->AddEffect<BloomEffect>()->SetThreshold(0.9); 

    Scene::RegisterComponent<FPSCamera>(); 
    Scene::RegisterComponent<RotateTag>(); 
    Scene::RegisterComponent<Planet>(); 
    Scene::RegisterComponent<OrbitTag>(); 

    scene->AddSystem(new FPSCameraSystem()); 
    scene->AddSystem(new RotateSystem()); 
    scene->AddSystem(new WorldGenSystem()); 
    scene->AddSystem(new OrbitSystem()); 

    Mesh* mesh = Mesh::CreateCube(); 
    Material* mat = new Material(); 
    mat->SetShader(Shader::Load("Basic")); 

    Material* lightMat = new Material(); 
    lightMat->SetShader(Shader::Load("Basic")); 
    lightMat->SetVector4("fj_Emissive", {2.0, 2.0, 2.0, 1.0}); 

    GetWindow()->SetVSync(false); 
    // GetWindow()->SetMode(WindowMode::Fullscreen); 

    Entity planet; 
    {
        // create planet 
        planet = scene->CreateEntity(); 
        scene->AddComponent<Planet>(planet); 

        const int NumLights = 1; 
        for (int i = 0; i < NumLights; i++) 
        {
            Entity l = scene->CreateEntity(); 

            scene->GetComponent<Transform>(l).SetScale(Vector3(2.0)); 

            auto& orbit = scene->AddComponent<OrbitTag>(l); 
            orbit.Angle = (float) i / NumLights * FJ_2_PI; 
            orbit.Orbits = planet; 
            orbit.Distance = 125; 
            orbit.Speed = 1 * FJ_TO_RAD; 

            auto& mc = scene->AddComponent<MeshContainer>(l); 
            mc.SetMesh(mesh); 
            mc.SetMaterial(lightMat); 

            auto& light = scene->AddComponent<Light>(l); 
            light.SetType(LightType::Point); 
            light.SetColor(Vector4(Vector3(1.0) * 1, 1.0)); 
            light.SetRadius(100.0f); 
        }
    }

    Random r; 
    float range = 300; 
    for (int i = 0; i < 1000; i++)
    {
        // create cube 
        Entity e = scene->CreateEntity(); 
        auto& tfm = scene->GetComponent<Transform>(e); 
        auto& mc = scene->AddComponent<MeshContainer>(e); 
        auto& rot = scene->AddComponent<RotateTag>(e); 
        mc.SetMesh(mesh); 
        mc.SetMaterial(mat); 
        tfm.SetPosition({
            r.NextFloat() * range - range * 0.5f, 
            r.NextFloat() * range - range * 0.5f, 
            r.NextFloat() * range - range * 0.5f 
        });
        rot.Amount = r.NextFloat() * 1; 

        auto& orbit = scene->AddComponent<OrbitTag>(e); 
        orbit.Angle = r.NextFloat() * FJ_2_PI; 
        orbit.Orbits = planet; 
        orbit.Distance = r.NextFloat() * 1000 + 20; 
        orbit.Speed = 400 / orbit.Distance * FJ_TO_RAD; 
    }

    {
        // create camera 
        Entity e = scene->CreateEntity(); 
        auto& tfm = scene->GetComponent<Transform>(e); 
        auto& cam = scene->AddComponent<Camera>(e); 
        // auto& light = scene->AddComponent<Light>(e); 
        scene->AddComponent<FPSCamera>(e); 
        
        tfm.SetPosition(Vector3::Backward * 20); 
        cam.SetFOV(70.0f); 

        // light.SetType(LightType::Point); 
        // light.SetColor(Color::White); 
        // light.SetRadius(100.0f); 
    }
}

void Main::PreUpdate(float dt) 
{
    (void) dt; 
    auto* input = GetInput(); 

    if (input->GetKey(KeyEscape)) 
    {
        GetWindow()->SetGrabMouse(false); 
    }
    if (input->GetButton(1)) 
    {
        GetWindow()->SetGrabMouse(true); 
    }
    if (input->GetKey(KeyEnter)) 
    {
        FJ_DEBUG("enter"); 
        Fjord::Stop(); 
    }
}

void Main::PreRender() 
{
    GetRenderer()->SetAmbientColor(Vector4(Vector3(1) * 0.1, 1.0)); 
}

void Main::PostRender() 
{

}

ENGINE_MAIN_CLASS(Main) 
