#include "WorldGen/Systems/WorldGenSystem.h" 

#include "Fjord/Graphics/PostProcess.h" 
#include "Fjord/Graphics/Renderer.h" 

#include "WorldGen/Components.h" 
#include "WorldGen/WorldGenThread.h" 

WorldGenSystem::WorldGenSystem() 
{
    IncludeComponent<Planet>(); 
    IncludeComponent<Transform>(); 
}

void WorldGenSystem::UpdateGUI() 
{
    auto* scene = GetScene(); 

    bool gen = false; 
    UI::BeginWindow("WorldGen", UI::WindowFlag::WindowFlagAutoResize); 
    if (UI::Button("Generate")) gen = true; 
    if (UI::Button("Quit")) Fjord::Stop(); 
    UI::EndWindow(); 

    for (Entity e : GetEntities()) 
    {
        Planet& planet = scene->GetComponent<Planet>(e); 
        
        // UI::PushId(i++); 
        
        if (planet.GenThread && planet.GenThread->IsRunning()) 
        {
            // UI::Button("Generating..."); 
        }
        else 
        {
            if (planet.GenThread) 
            {
                planet.World = planet.GenThread->World; 
                
                Material* mat = new Material(); 
                mat->SetShader(Shader::Load("Basic")); 
                planet.MainMaterial = mat; 

                planet.CellMesh = new Mesh(); 
                planet.GenThread->CellMeshData->Apply(planet.CellMesh); 

                planet.GenThread = nullptr; 
            }

            if (gen || planet.Empty) //UI::Button("Generate")) 
            {
                planet.Empty = false; 
                planet.GenThread = new WorldGenThread(7); 
                planet.GenThread->Start(); 
            }
        }
        
        // UI::PopId(); 
        
    }
    // UI::EndWindow(); 
}

void WorldGenSystem::Update(float dt) 
{
    auto* scene = GetScene(); 

    Quaternion rot = Quaternion::AxisAngle(Vector3::Up, 0.0 * dt); 

    for (Entity e : GetEntities()) 
    {
        auto& tfm = scene->GetComponent<Transform>(e); 
        tfm.SetRotation(tfm.GetRotation() * rot); 
    }
}

void WorldGenSystem::Render() 
{
    auto* scene = GetScene(); 
    auto* renderer = GetRenderer(); 

    for (Entity e : GetEntities()) 
    {
        auto& planet = scene->GetComponent<Planet>(e); 
        auto& tfm = scene->GetComponent<Transform>(e); 
        tfm.SetScale(Vector3(10)); 

        if (planet.CellMesh) 
        {
            // renderer->DrawMesh(
            //     planet.CellMesh, 
            //     planet.MainMaterial, 
            //     tfm.GetMatrix() 
            // ); 
        }
    }
}