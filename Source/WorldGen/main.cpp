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
#include <Fjord/Graphics/Renderer.h> 
#include <Fjord/Graphics/RenderSystems.h> 
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

#include <iostream> 

using namespace Fjord; 
using namespace std; 

class Main : public Application 
{
public: 
    virtual void Init() override; 
    virtual void UpdateGUI(float dt) override; 
    virtual void Update(float dt) override; 
    virtual void Render() override; 

    Ref<Fjord::Scene> Scene; 
}; 

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

class Planet : public Component 
{
public: 
    class World World; 
    Ref<Material> MainMaterial; 
    Ref<Mesh> CellMesh, OutlineMesh; 
};

class WorldGenSystem : public EntitySystem 
{
public: 
    WorldGenSystem() 
    {
        IncludeComponent<Planet>(); 
        IncludeComponent<Transform>(); 
    }

    virtual void UpdateGUI() override 
    {
        auto* scene = GetScene(); 

        for (Entity e : GetEntities()) 
        {
            UI::BeginWindow("WorldGen", UI::WindowFlag::WindowFlagAutoResize); 
            if (UI::Button("Generate")) 
            {
                GenWorld(scene->GetComponent<Planet>(e), 5); 
            }
            UI::EndWindow(); 
        }
    };

    virtual void Render() override 
    {
        auto* scene = GetScene(); 
        auto* renderer = GetRenderer(); 

        for (Entity e : GetEntities()) 
        {
            auto& planet = scene->GetComponent<Planet>(e); 
            auto& tfm = scene->GetComponent<Transform>(e); 
            tfm.SetScale(Vector3(100)); 

            if (planet.CellMesh) 
            {
                renderer->DrawMesh(
                    planet.CellMesh, 
                    planet.MainMaterial, 
                    tfm.GetMatrix() 
                ); 
            }
        }
    };

private: 
    void GenWorld(Planet& planet, unsigned worldSize) 
    {
        WorldGenerator gen; 
        gen.AddRule(new SubdivideCellGenRule(worldSize)); 
        if (worldSize > 1) gen.AddRule(new CellDistortRule()); 
        gen.AddRule(new CellRelaxRule(50)); 
        gen.AddRule(new BasicTerrainGenRule()); 
        // gen.AddRule(new ClimateSimulationRule(70, 200)); 
        class World w = gen.Generate();

        planet.World = w; 
        GenMeshData(planet); 
    }

    void GenMeshData(Planet& planet) 
    {
        Material* mat = new Material(); 
        mat->SetShader(Shader::Load("Basic")); 
        planet.MainMaterial = mat; 

        World& world = planet.World; 

        unsigned neighborCount = 0; 
        Vector<unsigned> neighborIndex; neighborIndex.reserve(world.GetConnectionCount()); 
        for (CellId i = 0; i < world.GetCellCount(); i++) 
        {
            neighborIndex.push_back(neighborCount); 
            neighborCount += world.GetNeighborCount(i); 
        }

        // cells 
        {
            Ref<MeshData> cellMeshData = new MeshData(); 
            Vector<Vector3>& verts = cellMeshData->Vertices; 
            Vector<Vector3>& normals = cellMeshData->Normals; 
            Vector<Vector4>& colors = cellMeshData->Colors; 
            Vector<uint32>& inds = cellMeshData->Indices; 
            // verts.reserve(World.GetCellCount() * 6); 
            // colors.reserve(World.GetCellCount() * 6); 
            // normals.reserve(World.GetCellCount() * 6); 
            // inds.reserve(World.GetCellCount() * 6 * 3); 
            verts.resize(neighborCount); 
            normals.resize(neighborCount); 
            colors.resize(neighborCount); 
            inds.resize((neighborCount - world.GetCellCount()*2) * 3); 
            // for (CellId i = 0; i < World.GetCellCount(); i++) 
            ParallelFor(CellId(0), CellId(world.GetCellCount()), [&](CellId i)
            {
                Vector3 normal = world.GetPosition(i); 
                Vector<Vector3> bounds = world.GetBounds(i); 
                unsigned start = neighborIndex[i]; //verts.size(); 
                unsigned index = start; 
                for (Vector3& v : bounds) 
                {
                    verts[index] = v; 
                    normals[index] = normal; 
                    colors[index++] = GetTerrainColor(world.GetTerrain(i)); 
                }

                index = start + 1; 
                
                unsigned ind = (neighborIndex[i] - i*2) * 3; 
                for (unsigned v = 0; v < bounds.size() - 2; v++) 
                {
                    inds[ind++] = (start); 
                    inds[ind++] = (index++); 
                    inds[ind++] = (index); 
                }
            }); 
            planet.CellMesh = new Mesh(); 
            cellMeshData->Apply(planet.CellMesh); 
            planet.CellMesh->CalculateNormals(); 
        }
    }
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

        Vector3 move; 
        if (input->GetKey(KeyI)) move += Vector3::Forward; 
        if (input->GetKey(KeyK)) move += Vector3::Backward; 
        if (input->GetKey(KeyJ)) move += Vector3::Left; 
        if (input->GetKey(KeyL)) move += Vector3::Right; 
        if (input->GetKey(KeyH)) move += Vector3::Up; 
        if (input->GetKey(KeyN)) move += Vector3::Down; 
        move *= MoveSpeed * dt; 

        float rotRL = 0; 
        float rotUD = 0; 
        if (input->GetKey(KeyW)) rotUD += 1; 
        if (input->GetKey(KeyS)) rotUD -= 1; 
        if (input->GetKey(KeyA)) rotRL += 1; 
        if (input->GetKey(KeyD)) rotRL -= 1; 
        rotRL *= RotSpeed * dt; 
        rotUD *= RotSpeed * dt; 

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

void Main::Init() 
{
    Fjord::Scene::RegisterComponent<FPSCamera>(); 
    Fjord::Scene::RegisterComponent<RotateTag>(); 
    Fjord::Scene::RegisterComponent<Planet>(); 

    Scene = new Fjord::Scene(); 

    Scene->AddSystem(new FPSCameraSystem()); 
    Scene->AddSystem(new RotateSystem()); 
    Scene->AddSystem(new WorldGenSystem()); 
    Scene->AddSystem(new MeshRenderSystem()); 
    Scene->AddSystem(new LightRenderSystem());
    Scene->AddSystem(new CameraRenderSystem()); 

    Mesh* mesh = Mesh::CreateCube(); 
    Material* mat = new Material(); 
    mat->SetShader(Shader::Load("Basic")); 

    Material* lightMat = new Material(); 
    lightMat->SetShader(Shader::Load("Basic")); 
    lightMat->SetVector4("fj_Emissive", {1.0, 1.0, 1.0, 1.0}); 

    GetWindow()->SetVSync(false); 

    Random r; 
    float range = 50; 
    for (int i = 0; i < 1000; i++)
    {
        // create cube 
        Entity e = Scene->CreateEntity(); 
        auto& tfm = Scene->GetComponent<Transform>(e); 
        auto& mc = Scene->AddComponent<MeshContainer>(e); 
        auto& rot = Scene->AddComponent<RotateTag>(e); 
        mc.SetMesh(mesh); 
        mc.SetMaterial(mat); 
        tfm.SetPosition({
            r.NextFloat() * range - range * 0.5f, 
            r.NextFloat() * range - range * 0.5f, 
            r.NextFloat() * range - range * 0.5f 
        });
        rot.Amount = r.NextFloat() * 1; 
    }

    {
        // create camera 
        Entity e = Scene->CreateEntity(); 
        auto& tfm = Scene->GetComponent<Transform>(e); 
        auto& cam = Scene->AddComponent<Camera>(e); 
        auto& light = Scene->AddComponent<Light>(e); 
        Scene->AddComponent<FPSCamera>(e); 
        
        tfm.SetPosition(Vector3::Backward * 5); 
        cam.SetFOV(70.0f); 

        light.SetType(LightType::Point); 
        light.SetColor(Color::White); 
        light.SetRadius(100.0f); 
    }

    {
        // create planet 
        Entity e = Scene->CreateEntity(); 
        Scene->AddComponent<Planet>(e); 
    }
}

void Main::UpdateGUI(float dt) 
{
    (void) dt; 

    // UI::BeginWindow("Debug", UI::WindowFlag::WindowFlagAutoResize); 
    // if (UI::Button("Quit")) Fjord::Stop(); 
    // UI::EndWindow(); 

    Scene->UpdateGUI(); 
}

void Main::Update(float dt) 
{
    Scene->Update(dt); 
}

void Main::Render() 
{
    Scene->Render(); 
}

ENGINE_MAIN_CLASS(Main) 
